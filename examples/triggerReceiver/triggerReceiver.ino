#include <SerialCommander.h>

//Trigger send value on the communication when is value change
Transmission comander = Transmission();

void myTriggerAndValueCallback(Request request) {
    if (request.command == Command_Trigger_return) {
        Serial.print("Receiving data from a Trigger ! pin :");
        Serial.print(request.header);
        Serial.print(" with the value :");
        Serial.print(request.body);
        Serial.println();
    }
}

void setup() {
    Serial.begin(9600);
    comander.enableLog(true);
    comander.setStream(&Serial);
    comander.setTimeout(5000);
    comander.setValueCallback(&myTriggerAndValueCallback);
}

void loop() {
    comander.handler();
}
