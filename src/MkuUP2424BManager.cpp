#include <MkuUP2424BManager.h>
#include <cstdio>
#include <stacktrace>

MkuUP2424Manager::MkuUP2424Manager(HardwareSerial *serial, int receiveWaitTime, bool verboseLogging)
{
    _serial = serial;
    _receiveWaitTime = receiveWaitTime;
    _verboseLogging = verboseLogging;
}

void MkuUP2424Manager::LogSpacer(const char *caller, const int line)
{
    if (_verboseLogging)
    {
        LogMessage("", LogLevel::UNDEFINED, caller, line);
        LogMessage("==================================================", LogLevel::UNDEFINED, caller, line);
        LogMessage("", LogLevel::UNDEFINED, caller, line);
    }
}

char* MkuUP2424Manager::saprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int cnt = vsprintf(nullptr, format, args);
    auto buf = (char *)malloc(cnt);

    vsprintf(buf, format, args);

    va_end(args);

    return buf;
}

void MkuUP2424Manager::LogMessagePrintf(const char *msg, const char *caller, const int line, LogLevel logLevel, ...)
{
    va_list args;
    va_start(args, msg);

    if (_verboseLogging)
    {
        int cnt = vsprintf(nullptr, msg, args);

        auto buf = (char *)malloc(cnt);

        vsprintf(buf, msg, args);

        LogMessage(buf, logLevel, caller, line);

        free(buf);
    }

    va_end(args);
}

void MkuUP2424Manager::LogMessage(const char *msg, LogLevel logLevel, const char *caller, const int line)
{
    if (_verboseLogging)
    {
        Serial.printf("[%f] [%s]: Module: %s, Method: %s, Line: %d: %s\n", (double)micros() / 1000000.0, nameof::nameof_enum<LogLevel>(logLevel).data(), nameof::nameof_full_type<MkuUP2424Manager>().data(), caller, line, msg);
    }
}

void MkuUP2424Manager::Init(int baud)
{
    LogMessage("Initializing");

    _serial->begin(baud);
}

int MkuUP2424Manager::readByteWithTimeout(int timeout)
{
    auto start = millis();

    while (start + timeout > millis())
    {
        if (_serial->available())
        {
            auto res = _serial->read();

            if (res != 255)
            {
                // discharge carriage return
                auto val = _serial->read();

                if (val != '\r')
                {
                    LogMessagePrintf("Invalid end-of-response character! Dec: %d str: '%c'", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::ERROR, val, (char) val);
                }

                return res;
            }
        }
    }

    return -1;
}

void MkuUP2424Manager::TrySendConfigCommand(const char *command)
{
    _serial->write(command, strlen(command));
    _serial->print('\r');
}

void MkuUP2424Manager::TrySetMode(TransmitterMode mode)
{
    const char* cmd = nullptr;
    switch(mode)
    {
        case TransmitterMode::RX:
            cmd = "P0";
            break;
        case TransmitterMode::TX:
            cmd = "P1";
            break;
        default:
            LogMessage("Panic: Invalid TransmitterMode!");
            panic("Invalid TransmitterMode!");
    }

    TrySendConfigCommand(cmd);
}

void MkuUP2424Manager::TrySetPowerState(PowerState state)
{

    const char *cmd = nullptr;
    switch (state)
    {
    case TransmitterMode::RX:
        cmd = "O0";
        break;
    case TransmitterMode::TX:
        cmd = "O1";
        break;
    default:
        LogMessage("Panic: Invalid PowerState!");
        panic("Invalid PowerState!");
    }

    TrySendConfigCommand(cmd);
}