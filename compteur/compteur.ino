
#include <Arduino.h>
#define PIN_ENC_A_CHAN_A 2 // interrupt 0
#define PIN_ENC_A_CHAN_B 5

#define PIN_ENC_B_CHAN_A 3 // interrupt 1
#define PIN_ENC_B_CHAN_B 4

#define SEPARATOR ':'
#define SAMPLE_TIME 100 // ms

#define SEQUENCE_MAX_SIZE 30000

unsigned long lastSampleTime;
unsigned int sampleSequence;

volatile long counterA = 0L;
volatile long counterB = 0L;

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
	sampleSequence = 0;
}

void loop() {
	if(millis() - lastSampleTime >= SAMPLE_TIME) {
    	Serial.print(sampleSequence, DEC);
    	Serial.print(SEPARATOR);
		Serial.print(counterA, DEC); // Motor C, B
		Serial.print(SEPARATOR);
		Serial.println(counterB, DEC); // Motor D, A
		counterA = 0L;
		counterB = 0L;
		lastSampleTime = millis();
	    sampleSequence++;
	    if(sampleSequence >= SEQUENCE_MAX_SIZE) { // On gère manuellement le débordement
	    	sampleSequence = 0;
	    }
	}
}