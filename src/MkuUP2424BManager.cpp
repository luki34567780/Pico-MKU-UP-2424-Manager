#include <MkuUP2424BManager.h>
#include <cstdio>

MkuUP2424Manager::MkuUP2424Manager(HardwareSerial *serial, int receiveWaitTime, bool verboseLogging)
{
    _serial = serial;
    _receiveWaitTime = receiveWaitTime;
    _verboseLogging = verboseLogging;
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

void MkuUP2424Manager::LogMessagePrintf(const char *msg, ...)
{
    if (_verboseLogging)
    {
        const char *caller = __builtin_FUNCTION();
        const int line = __builtin_LINE();
        va_list args;
        va_start(args, msg);

        int cnt = vsprintf(nullptr, msg, args);

        auto buf = (char *)malloc(cnt);

        vsprintf(buf, msg, args);

        Serial.printf("Module: %s, Method: %s, Line: %d: %s\n", nameof::nameof_full_type<MkuUP2424Manager>().data(), caller, line, buf);

        free(buf);
    }
}

void MkuUP2424Manager::LogMessage(const char *msg, const char *caller = __builtin_FUNCTION(), const int line = __builtin_LINE())
{
    if (_verboseLogging)
    {
        Serial.printf("Module: %s, Method: %s, Line: %d: %s\n", nameof::nameof_full_type<MkuUP2424Manager>().data(), caller, line, msg);
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
    sendCommand("p");

    auto val = _serial->read();

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
                Serial.printf("Module: %s, Method: %s, Line: %d: GetMode() serial response value is bad! Expected %d or %d, got %d\n", nameof::nameof_full_type<MkuUP2424Manager>().data(), "GetMode", __builtin_LINE(), '0', '1', (byte)val);
            }
            return TransmitterMode::TRANSMITTER_ERROR;
    }
}

void MkuUP2424Manager::sendCommand(const char *command)
{
    _serial->write(command, strlen(command));
    _serial->write('\r');
}

byte MkuUP2424Manager::GetForwardPower()
{
    LogMessage("Getting forward power");
    sendCommand("f");
    
    delay(_receiveWaitTime);

    return _serial->readString().toInt();
}

byte MkuUP2424Manager::GetReversePower()
{
    LogMessage("Getting reverse power");
    sendCommand("r");

    delay(_receiveWaitTime);

    return _serial->readString().toInt();
}

PowerState MkuUP2424Manager::GetPowerState()
{
    LogMessage("Getting power state");
    sendCommand("o");

    auto val = readByteWithTimeout(1000);

    switch (val)
    {
        case '0':
            return PowerState::OFF;
        case '1':
            return PowerState::ON;
        default:
            LogMessagePrintf("GetPowerState() serial response value is bad! Expected %d or %d, got %d", '0', '1', val);
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
            return _serial->read();
        }
    }

    return -1;
}

bool MkuUP2424Manager::TrySendConfigCommand(const char *command)
{
    _serial->write(command, strlen(command));
    _serial->write("\r");

    auto resp = readByteWithTimeout(1000);

    LogMessagePrintf("Command Response: %d\n", resp);

    //     auto buf = (char *)malloc(sprintf(nullptr, ));
    // sprintf(buf, "Command Response: %d\n", resp);
    // LogMessage(buf);
    // free(buf);

    return resp == 'A';
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