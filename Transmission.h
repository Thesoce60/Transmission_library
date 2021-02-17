#ifndef SERIAL_COMMANDER_H
#define SERIAL_COMMANDER_H

#include "Command_map.h"
#include "Arduino.h"

#ifndef COMMAND_BUFFER
#define COMMAND_BUFFER 4
#endif
#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH sizeof(long)*COMMAND_BUFFER
#endif
#define TRIGGER_POOL_LENGTH 10
typedef struct {
    uint8_t command;
    uint8_t header;
    uint16_t body;
} Request;
typedef struct {
    uint8_t state;
    uint16_t value;
} Result;
typedef struct {
    uint8_t trigger_pin;
    uint16_t trigger_value;
    bool digital;
} Trigger;

class Transmission {
public :

    Transmission();

    void setValueCallback(void (*callback)(Request request));

    //Params :
    void enableLog(bool state);

    void setTimeout(long timeout);

    void setStream(Stream *stream);

    bool addTrigger(uint8_t pin, bool digital);

    bool removeTrigger(uint8_t pin);

    //Routine :
    void handler();

    //
    int available();

    void decodeResponse(uint8_t responseState);

    //Command constructor :
    uint32_t getPinDeclarationCommand(uint8_t pin, uint8_t state);

    uint32_t getPinSateChangeCommand(uint8_t pin, bool state);

    uint32_t getAnalogReadCommand(uint8_t pin);

    uint32_t getPWMChange(uint8_t pin, uint8_t value);

    //Sync command sender :
    Result sendPinDeclaration(uint8_t pin, uint8_t state);

    Result sendPinState(uint8_t pin, bool state);

    Result sendAnalogReadRequest(uint8_t pin);

    Result sendPWMChange(uint8_t pin, uint8_t value);

    Result sendSyncValue(uint8_t header, uint16_t value);

    void sendValue(uint8_t header, uint16_t value);

    void sendResult(Result *result);

    //executor
    Result execute(Request request);

    //Other
    bool commandExist(uint8_t command);

private :
    Trigger *_triggers[TRIGGER_POOL_LENGTH];
    uint8_t _triggerPos = 0;
    bool _log;
    long _timeout = 1000;
    Stream *_stream;

    void (*_callback)(Request request);

    Request _parseRequest(uint8_t *buffer);

    Result _syncSenderStateReturn(uint32_t command);

    void _send(uint32_t command);

    uint32_t _generateCommand(uint8_t command, uint8_t headerCommand, uint16_t commandData);

    void print(char *info);

    void printHelp(const char *help);
};

// HINTS STRINGS;
static const char *helpLog_UNKSTREAM = "Error, Impossible to send data on a NULL stream refer Stream with setStream(Stream* stream)";
static const char *helpLog_DISABLEDLOG = "Log disabled, enable with : enableLog(bool log)";
static Result errorResult = {State_ERR, 0};
#endif