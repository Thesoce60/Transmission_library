#include <Transmission.h>

Transmission comander = Transmission();

void setup() {
    Serial.begin(9600);
    comander.enableLog(true);
    comander.setStream(&Serial);
    comander.setTimeout(5000);
    Result response = comander.sendPinDeclaration(13, OUTPUT);
    if (response.state == State_TIMEOUT) {
        while (1);// the receiver haven't awnser the query after the timeout time, Waiting for restart
    }
}

void loop() {
    comander.sendPinState(13, HIGH);
    delay(500);
    comander.sendPinState(13, LOW);
    delay(500);
}
