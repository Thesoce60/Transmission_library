# Transmission
This Arduino library is for communicating command between boards with a serial communication bus (inherit of a Stream).
You can also send commands without classic stream with decode and encode functions.
## Commands ?
 Yes, pinMode, analogWrite, analogRead, digitalRead, digitalWrite. This commands and many others can be sent over the bus to change another Arduino state who listening on the bus.

## Get Started
- With Serial Communications
	```mermaid
		graph LR
		A(Card 1) ---|TX|B(Card 2)
		B ---|RX|A
		A ---|GND|B
	​```
- With I2C
	```mermaid
		graph LR
		A(Card 1) ---|SCL|B(Card 2)
		B ---|SDA|A
		A ---|GND|B
	​```
### Emiter 
```c++
#include <Transmission.h>

Transmission comander = Transmission();

void setup() {
    Serial.begin(9600); // init the communication
    comander.enableLog(false); // enable/disable log (dont use the same Serial for log and Transmission) 
    comander.setStream(&Serial);
    comander.setTimeout(1000); //add timeout for sync send
    Result response = comander.sendPinDeclaration(13, OUTPUT);// send synced pinMode
    if (response.state == State_TIMEOUT) {
        while (1);
    }
}

void loop() {
    comander.sendPinState(13, HIGH);//send synced 
    delay(500);
    comander.sendPinState(13, LOW);
    delay(500);
}
```
### Receiver
```c++
#include <Transmission.h>

Transmission comander = Transmission();

void setup() {
    Serial.begin(9600);
    comander.setStream(&Serial);
}

void loop() {
    comander.handler();// waiting for incomming command request
}
```

## Adding triggers
This is possible to add triggers if a pin state change the data will be sent over the bus.
- Add triggers
	```c++
	#include <Transmission.h>

	Transmission comander = Transmission();
	byte pin = 3;
	void setup() {
	    Serial.begin(9600);
	    comander.setStream(&Serial);
	    pinMode(pin ,INPUT_PULLUP);
	    commander.addTrigger(pin , true); // adding digital trigger on pin 3 and 		true for digital trigger (change from low to high or high to low);
	    commander.addTrigger(A0, false); // adding analog trigger on pin A0
	    commander.removeTrigger(A0); // remove the trigger on A0 pin
	}

	void loop() {
	    comander.handler();// waiting for incomming command request
	}
	```
	The max trigger size is 10, defined by the value **TRIGGER_POOL_LENGTH**
- Receive a trigger data
	```c++
	#include <Transmission.h>

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
	    comander.setStream(&Serial);
	    comander.setValueCallback(&myTriggerAndValueCallback);
	}

	void loop() {
	    comander.handler();
	}
	```
## Sending/Receiving custom values
```c++
 //...some awmesome code
 comander.sendSyncValue(unit8_t headerValue,uint16_t value);
```
To receive custom values
```c++
#include <Transmission.h>

Transmission comander = Transmission();

void myTriggerAndValueCallback(Request request) {
    if (request.command == Command_value) {
        Serial.print("Receiving data value data ! header :");
        Serial.print(request.header);
        Serial.print(" with the value :");
        Serial.print(request.body);
        Serial.println();
    }
}

void setup() {
    Serial.begin(9600);
    comander.setStream(&Serial);
    comander.setValueCallback(&myTriggerAndValueCallback);
}

void loop() {
    comander.handler();
}
```
# Todo
- Add communication channel
- Add communication error corrector.
- Create examples for Wifi connections and WebSockets
- Create library in other language for web integration, app integration.
# Other
Any  problem  or  question?  Post  an  Issue!
