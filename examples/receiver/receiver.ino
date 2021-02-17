#include <SerialCommander.h>

Transmission comander = Transmission();

void setup() {
    Serial.begin(9600);
    comander.enableLog(true);
    comander.setStream(&Serial);
    comander.setTimeout(5000);
}

void loop() {
    comander.handler();
}
