#include <SerialCommander.h>

//Trigger send value on the communication when is value change
Transmission comander = Transmission();

void setup() {
    Serial.begin(9600);
    comander.enableLog(true);
    comander.setStream(&Serial);
    comander.setTimeout(5000);
    commander.addTrigger(3, true); // adding digital trigger on pin 3;
}

void loop() {
    comander.handler();
}
