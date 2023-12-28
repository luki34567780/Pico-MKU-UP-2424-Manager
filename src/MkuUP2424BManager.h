#pragma once

#include <nameof.hpp>
#include <Arduino.h>

enum TransmitterMode
{
    RX,
    TX,
    TRANSMITTER_ERROR
};

enum PowerState
{
    OFF,
    ON,
    POWERSTATE_ERROR
};

class MkuUP2424Manager
{
public:
    MkuUP2424Manager(HardwareSerial *serial, int receiveWaitTime, bool verboseLogging);
    void Init(int baud, int rxPin, int txPin);
    void Init(int baud);
    TransmitterMode GetMode();
    byte GetForwardPower();
    byte GetReversePower();
    PowerState GetPowerState();
    bool IsPoweredOn();
    double GetTemperature();
    bool TrySetMode(TransmitterMode mode);
    bool TrySetPowerState(PowerState state);
private:
    void LogMessage(const char *msg, const char *caller, const int line);
    bool TrySendConfigCommand(const char* command);
    void sendCommand(const char *command);
    HardwareSerial *_serial;
    int _receiveWaitTime;
    int readByteWithTimeout(int timeout);
    bool _verboseLogging;
    void LogMessagePrintf(const char *msg, ...);
    char *saprintf(const char *format, ...);
};