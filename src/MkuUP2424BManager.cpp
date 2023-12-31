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

void MkuUP2424Manager::Init(int baud, int rxPin, int txPin)
{
    LogMessage("Initializing");

    ((SerialUART*)_serial)->setRX(rxPin);
    ((SerialUART*)_serial)->setTX(txPin);

    _serial->begin(baud);
}

void MkuUP2424Manager::Init(int baud)
{
    LogMessage("Initializing");

    _serial->begin(baud);
}

TransmitterMode MkuUP2424Manager::GetMode()
{
    LogMessage("GetMode() called");
    sendCommand('p');

    auto val = readByteWithTimeout(_receiveWaitTime);

    switch (val)
    {
        case '0':
        LogMessage("Mode: RX");
            return TransmitterMode::RX;
        case '1':
            LogMessage("Mode: TX");
            return TransmitterMode::TX;
        default:
            if (_verboseLogging)
            {
                LogMessagePrintf("GetMode() serial response value is bad! Expected %d or %d, got dec: %d str: '%c'", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::ERROR, '0', '1', val, (char)val);
            }
            return TransmitterMode::TRANSMITTER_ERROR;
    }
}

void MkuUP2424Manager::sendCommand(const char command)
{
    _serial->print(command);
    _serial->print('\r');
}

double MkuUP2424Manager::GetForwardPower()
{
    LogMessage("Getting forward power");
    sendCommand('f');

    auto str = _serial->readStringUntil('\r');
    auto val = str.toDouble();

    LogMessagePrintf("Got forward power, string: '%s' decimal: '%f", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::DEBUG, str, val);

    return val;
}

double MkuUP2424Manager::GetReversePower()
{
    LogMessage("Getting reverse power");
    sendCommand('r');

    auto str = _serial->readStringUntil('\r');
    auto val = str.toDouble();

    LogMessagePrintf("Got reverse power, string: '%s' decimal: '%f", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::DEBUG, str, val);

    return val;
}

PowerState MkuUP2424Manager::GetPowerState()
{
    LogMessage("Getting power state");
    sendCommand('o');

    auto val = readByteWithTimeout(_receiveWaitTime);

    switch (val)
    {
        case '0':
            return PowerState::OFF;
        case '1':
            return PowerState::ON;
        default:
            LogMessagePrintf("GetPowerState() serial response value is bad! Expected %d or %d, got %d", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::ERROR, '0', '1', val);
            return PowerState::POWERSTATE_ERROR;
    }
}

bool MkuUP2424Manager::IsPoweredOn()
{
    return GetPowerState() == PowerState::ON;
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

bool MkuUP2424Manager::TrySendConfigCommand(const char *command)
{
    _serial->write(command, strlen(command));
    _serial->print('\r');

    auto val = readByteWithTimeout(_receiveWaitTime);

    LogMessagePrintf("Command Response: dec: %d str: '%c'", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::DEBUG, val, (char)val);

    return val == 'A';
}

bool MkuUP2424Manager::TrySetMode(TransmitterMode mode)
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

    return TrySendConfigCommand(cmd);
}

bool MkuUP2424Manager::TrySetPowerState(PowerState state)
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

    return TrySendConfigCommand(cmd);
}