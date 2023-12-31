#include <MkuUP2424BManager.h>

MkuUP2424Manager::MkuUP2424Manager(HardwareSerial *serial, bool verboseLogging)
{
    _serial = serial;
    _verboseLogging = verboseLogging;
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

void MkuUP2424Manager::SendConfigCommand(const char *command)
{
    _serial->write(command, strlen(command));
    _serial->print('\r');
}

void MkuUP2424Manager::SetMode(TransmitterMode mode)
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

    SendConfigCommand(cmd);
}

void MkuUP2424Manager::SetPowerState(PowerState state)
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

    SendConfigCommand(cmd);
}