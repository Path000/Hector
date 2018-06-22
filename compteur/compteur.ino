
#include <Arduino.h>
#define PIN_ENC_A_CHAN_A 2 // interrupt 0
#define PIN_ENC_A_CHAN_B 5

#define PIN_ENC_B_CHAN_A 3 // interrupt 1
#define PIN_ENC_B_CHAN_B 4

#define SEPARATOR ':'
#define SAMPLE_TIME 100 // ms

/*
	Cette version n'est pas prévue pour l'odométrie.
	Uniquement pour avoir une vitesse instantanée des roues.
*/

unsigned long lastSampleTime;

volatile unsigned int counterA = 0;
volatile unsigned int counterB = 0;

unsigned int speed1 = 0;
unsigned int speed2 = 0;

void interruptOnRaisingA() {
	counterA++;
}

void interruptOnRaisingB() {
	counterB++;
}

void setup() {
	Serial.begin(115200);

	pinMode(PIN_ENC_A_CHAN_A, INPUT);
	pinMode(PIN_ENC_B_CHAN_A, INPUT);
	pinMode(PIN_ENC_A_CHAN_B, INPUT);
	pinMode(PIN_ENC_B_CHAN_B, INPUT);

	attachInterrupt(digitalPinToInterrupt(PIN_ENC_A_CHAN_A), interruptOnRaisingA, RISING);
	attachInterrupt(digitalPinToInterrupt(PIN_ENC_B_CHAN_A), interruptOnRaisingB, RISING);

	lastSampleTime = millis();
}

void loop() {
	if(millis() - lastSampleTime >= SAMPLE_TIME) {
		speed1 = counterA;
		speed2 = counterB;
		counterA = 0;
		counterB = 0;
		lastSampleTime = millis();
	}

	if(Serial.available() > 0) { // Trigger send with one octet. /!\ Arduino serial monitor sends more then one byte
		Serial.read(); // vide le buffer
		
		Serial.print(speed1); // Motor C, B
		Serial.print(SEPARATOR);
		Serial.println(speed2); // Motor D, A
		Serial.flush(); // Waits for the transmission of outgoing serial data to complete.
	}
}
