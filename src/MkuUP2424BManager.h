#pragma once

#include <nameof.hpp>
#include <Arduino.h>

enum TransmitterMode
{
    RX,
    TX
};

enum PowerState
{
    OFF,
    ON
};

enum LogLevel
{
    UNDEFINED,
    CRITICAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    NONE
};

class MkuUP2424Manager
{
public:
    MkuUP2424Manager(HardwareSerial *serial, bool verboseLogging);
    void Init(int baud);
    void SetMode(TransmitterMode mode);
    void SetPowerState(PowerState state);
    void LogMessage(const char *msg, LogLevel logLevel = LogLevel::UNDEFINED, const char *caller = __builtin_FUNCTION(), const int line = __builtin_LINE());
    void LogMessagePrintf(const char *msg, const char *caller = __builtin_FUNCTION(), const int line = __builtin_LINE(), LogLevel logLevel = LogLevel::UNDEFINED, ...);
private:
    void SendConfigCommand(const char *command);
    void sendCommand(const char command);
    HardwareSerial *_serial;
    bool _verboseLogging;
};