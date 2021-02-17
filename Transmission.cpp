#include "Transmission.h"
#include "Command_map.h"
#include "Arduino.h"


Transmission::Transmission() {
    for (byte i = 0; i < TRIGGER_POOL_LENGTH; i++) {
        _triggers[i] = NULL;
    }
}

//----------------------Transmission Params-----------------
void Transmission::setStream(Stream *stream) {
    this->_stream = stream;
}

int Transmission::available() {
    return _stream->available();
}

void Transmission::enableLog(bool state) {
    this->_log = state;
}

void Transmission::setTimeout(long timeout) {
    _timeout = timeout;
}

void Transmission::setValueCallback(void (*callback)(Request)) {
    _callback = callback;
}

bool Transmission::addTrigger(uint8_t pin, bool digital) {
    Trigger trigger = Trigger();
    trigger.trigger_pin = pin;
    trigger.trigger_value = digitalRead(pin);
    trigger.digital = digital;
    for (byte i = 0; i < TRIGGER_POOL_LENGTH; i++) {
        if (_triggers[i] == NULL) {
            _triggers[i] = &trigger;
            return true;
        }
    }
    print("Trigger Pool is full try to up it with #define TRIGGER_POOL_LENGTH (value) default length is 10");
    return false;
}

bool Transmission::removeTrigger(uint8_t pin) {
    for (byte i = 0; i < TRIGGER_POOL_LENGTH; i++) {
        if (_triggers[i] != NULL)
            if (_triggers[i]->trigger_pin == pin) {
                _triggers[i] = NULL;
            }
    }
}

void Transmission::decodeResponse(uint8_t responseState) {
    if (!_log) return printHelp(helpLog_DISABLEDLOG);
    switch (responseState) {
        case State_ERR:
            print("(Response)The receiver can't perform the action");
            break;
        case State_TIMEOUT:
            print("(Response)The receiver haven't send result query data TIMED_OUT");
            break;
        case State_UNK:
            print("(Response)The receiver don't know the command UNKNOWN");
            break;
        case State_OK:
            print("(Response)All performed perfectly OK");
            break;
        default:
            print("Impossible to parse result");
            break;
    }
}

//-----------------------USAGE function
uint32_t Transmission::getPinDeclarationCommand(uint8_t pin, uint8_t state) {
    return _generateCommand(Command_Pin_Mode, pin, state);
}

uint32_t Transmission::getPinSateChangeCommand(uint8_t pin, bool state) {
    return _generateCommand(Command_Pin_Write, pin, state);
}

uint32_t Transmission::getAnalogReadCommand(uint8_t pin) {
    return _generateCommand(Command_Pin_Read, pin, 0);
}

uint32_t Transmission::getPWMChange(uint8_t pin, uint8_t value) {
    return _generateCommand(Command_PWM_Change, pin, value);
}

//
Result Transmission::sendPinDeclaration(uint8_t pin, uint8_t state) {
    uint32_t command = getPinDeclarationCommand(pin, state);
    return _syncSenderStateReturn(command); // send sync command max wait is _timeout value
}

Result Transmission::sendPinState(uint8_t pin, bool state) {
    uint32_t command = getPinSateChangeCommand(pin, state);
    return _syncSenderStateReturn(command);
}

Result Transmission::sendAnalogReadRequest(uint8_t pin) {
    return _syncSenderStateReturn(getAnalogReadCommand(pin));
}

Result Transmission::sendPWMChange(uint8_t pin, uint8_t value) {
    return _syncSenderStateReturn(getPWMChange(pin, value));
}

void Transmission::sendValue(uint8_t header, uint16_t value) {
    return _send(_generateCommand(Command_value, header, value));
}

Result Transmission::sendSyncValue(uint8_t header, uint16_t value) {
    return _syncSenderStateReturn(_generateCommand(Command_value, header, value));
}

// private
uint32_t Transmission::_generateCommand(uint8_t command, uint8_t headerCommand, uint16_t commandData) {
    return command << 24 | headerCommand << 16 | commandData;
}

void Transmission::handler() {
    if (_stream == NULL) {
        printHelp(helpLog_UNKSTREAM);
        return;
    }
    if (_stream->available()) {
        uint8_t buffer[4] = {};
        _stream->readBytes(buffer, sizeof(long));
        Request request = _parseRequest(buffer);
        if (commandExist(request.command)) {
            Result result = execute(request);
            print("Executing request");
            sendResult(&result);
        } else {
            print("Receiving bad request");
            sendResult(&errorResult);
        }
    }
    for (byte i = 0; i < TRIGGER_POOL_LENGTH; i++) {
        if (_triggers[i] != NULL) {
            uint16_t value = _triggers[i]->digital ? digitalRead(_triggers[i]->trigger_pin) : analogRead(
                    _triggers[i]->trigger_pin);
            if (value != _triggers[i]->trigger_value) {
                _triggers[i]->trigger_value = value;
                _send(_generateCommand(Command_Trigger_return, _triggers[i]->trigger_pin, _triggers[i]->trigger_value));
            }
        }
    }
}

bool Transmission::commandExist(uint8_t command) {
    return (command & 0xF0) == 0x00;
}

Result Transmission::execute(Request request) {
    Result output = Result();
    switch (request.command) {
        case Command_Pin_Mode:
            print("Pinmode");
            Serial.println(String("Setting ") + String(request.header) + String(" : ") + String(request.command));
            if (request.body < 3) {
                pinMode(request.header, request.body);
                output.state = State_OK;
                return output;
            }
            output.state = State_ERR;
            return output;
        case Command_Pin_Read:
            output.state = State_OK;
            output.value = digitalRead(request.header);
            return output;
        case Command_Pin_Write:
            print("PinWrite");
            output.state = State_OK;
            digitalWrite(request.header, request.body);
            return output;
        case Command_PWM_Change:
            output.state = State_OK;
            analogWrite(request.header, request.body);
            return output;
        case Command_Analog_Read:
            output.state = State_OK;
            output.value = analogRead(request.header);
            return output;
        case Command_Trigger_return:
        case Command_value:
            if (_callback != NULL) {
                (*_callback)(request);
                output.state = State_OK;
            } else {
                output.state = State_ERR;
                print("Getting value command (from trigger or sendValue()) but no value callback was initialized, init it with setValueCallback(void (*callback)(Request request));");
            }
            return output;
    }
    output.state = State_UNK;
    return output;
}

void Transmission::sendResult(Result *result) {
    char buffer[3] = {};
    buffer[0] = result->state;
    buffer[1] = result->value >> 8;
    buffer[2] = result->value & 0x00FF;
    _stream->write(buffer, 3);
}


//---------------------PRIVATE-----------------

void Transmission::print(char *info) {
    if (!_log)return;
    Serial.print('\n');
    Serial.print("[Transmission] : ");
    Serial.print(info);
}

void Transmission::printHelp(const char *help) {
    if (!_log)return;
    Serial.print('\n');
    Serial.print("[Transmission] : HELP :");
    Serial.print(help);
}

Result Transmission::_syncSenderStateReturn(uint32_t command) {
    if (_stream == NULL) {
        printHelp(helpLog_UNKSTREAM);
        return errorResult;
    }
    long t_start = millis();
    uint8_t buffer[4] = {command >> 24, command >> 16, command >> 8, command};
    _stream->write(buffer, sizeof(long));
    while ((millis() - t_start < _timeout)) {
        if (_stream->available() > 2) {
            Result result;
            result.state = _stream->read();
            result.value = _stream->read() << 8 | _stream->read();
            return result;
        }
        yield();
    }
    print("No response, timed out.");
    Result timeoutResult = {State_TIMEOUT, millis() - t_start};
    return timeoutResult;
}

void Transmission::_send(uint32_t command) {
    uint8_t buffer[4] = {command >> 24, command >> 16, command >> 8, command};
    _stream->write(buffer, sizeof(long));
}

Request Transmission::_parseRequest(uint8_t *buffer) {
    Request out = Request();
    out.command = buffer[0];
    out.header = buffer[1];
    out.body = buffer[2] << 8 | buffer[3];
    return out;
}