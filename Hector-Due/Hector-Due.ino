/*
----- PIN MAPPING -----

6	RPLidar MOTOCTL (PWM 25kHz)
3	Display RESET		
13	LED_BUILTIN
18	TX1
19	RX1 Serial1 RPLidar UART
20	SDA
21	SCL i2c display (@ 0x3D)

*/



#include <Scheduler.h>
#include "pwm_lib.h"

using namespace arduino_due::pwm_lib;

// pour le lidar -> 25kHz et 60% duty
// PWML7_PC24  // PWM_CH7 -> Le pin est le seul sur ce channel -> pin6
#define PWM_PERIOD_PIN_6 4000 // hundredth of usecs (1e-8 secs)
#define PWM_DUTY_PIN_6 2400 // hundredth of usecs (1e-8 secs)

// pour le driver moteur
pwm<pwm_pin::PWML7_PC24> pwm_pin6;

#define RESPONSE_HEAD_SIZE 7
#define CABIN_HEAD_SIZE 4
#define CABINS_SIZE 80

uint8_t RXResponseBuffer[RESPONSE_HEAD_SIZE];
uint8_t RXHeadBuffer[CABIN_HEAD_SIZE];
uint8_t RXCabinsBuffer[CABINS_SIZE];

const uint8_t requestStop[2] = {0xA5, 0x25};
const uint8_t requestExpressScan[9] = {0xA5, 0x82, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
const uint8_t responseHead[RESPONSE_HEAD_SIZE] = {0xA5, 0x5A, 0x54, 0x00, 0x00, 0x40, 0x82};

uint8_t bytesRead = 0;

struct HeadStruct {
	int8_t sync1; // New packet marker
	int8_t sync2;
	float startAngle; // current packet's startAngle and next's one are needed for current cabins
	uint8_t checkSum; // current packet's checksum
	uint8_t sum; // current head's sum 
	bool startFlag; // if true, current packet should be ignored
} head;

float startAngle;
uint8_t checkSum;
uint8_t sum;

struct CabinStruct {
	uint16_t distance1;
	uint16_t distance2;
	float deltaAngle1;
	float deltaAngle2;
} cabin;

struct ScanStruct {
	uint16_t distance;
	float angle;
	int16_t x;
	int16_t y;
};

// Store 1 full revolution
// 60% PWM => 0.9° resolution at 10Hz => 400 scan / turn (refering doc)
#define SAMPLE_BY_REVOLUTION 500
ScanStruct scans[SAMPLE_BY_REVOLUTION];
ScanStruct* scanPtr;
uint16_t scanIndex = 0;
uint16_t scanIndexTop = 0;
float lastAngle = 0;
float currentAngle = 0;



// Display
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define OLED_RESET 3
#define LINES 4

Adafruit_SSD1306 display(OLED_RESET);

uint32_t timestamp = 0;




void stopAll(String message) {
	// Arrête le moteur du lidar
	pwm_pin6.stop();
	// arrête le scan du lidar
	Serial1.write((const char *)requestStop);
	// print debug message
	Serial.println(message);
	//blink led
	digitalWrite(LED_BUILTIN, HIGH);
	// NB. avec le scheduler, le delay agit comme un yield.
	while(1);
}

void setup() {
	// Pour alerter
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Serial : 0 (RX) and 1 (TX) => Debug USB
	Serial.begin(115200);
	// Serial 1: 19 (RX) and 18 (TX) => RP Lidar
	Serial1.begin(115200);
	while(!Serial1);
	// Start PWM RP Lidar rotor
	pwm_pin6.start(PWM_PERIOD_PIN_6,PWM_DUTY_PIN_6);
	// Send express scan header
	Serial1.write((const char *)requestExpressScan);
	Serial1.setTimeout(3000);
	// read response
	bytesRead = Serial1.readBytes(RXResponseBuffer, RESPONSE_HEAD_SIZE);
	// test number of bytes read
	if(bytesRead != RESPONSE_HEAD_SIZE) stopAll("Timeout while reading express scan response head");
	// test response head value
	for(uint8_t i = 0; i++; i < RESPONSE_HEAD_SIZE) {
		if(responseHead[i] != RXResponseBuffer[i]) stopAll("Wrong express scan response head");
	}
	// read 1st cabin head
	bytesRead = Serial1.readBytes(RXHeadBuffer, CABIN_HEAD_SIZE);
	if(bytesRead != CABIN_HEAD_SIZE) stopAll("Timeout while reading cabin head");
	parseHead();
	if(head.sync1 != 0xA || head.sync2 != 0x5) stopAll("First two bytes should be a new packet");




	// Display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)

	Scheduler.startLoop(lidarRXParserLoop);
}

void parseHead() {
	uint8_t byte0 = RXHeadBuffer[0];
	uint8_t byte1 = RXHeadBuffer[1];
	uint8_t byte2 = RXHeadBuffer[2];
	uint8_t byte3 = RXHeadBuffer[3];

	head.sync1 = (byte0 & 0xF0) >> 4;
	head.sync2 = (byte1 & 0xF0) >> 4;
	head.checkSum = ((byte1 & 0x0F) << 4) | (byte0 & 0x0F);
	head.startAngle = (((byte3 & B01111111) << 8) | byte2) / 64;
	head.startFlag = !!byte3 & B10000000;
	head.sum = byte2 ^ byte3;
}

void parseCabin(uint8_t cabinNumber) {
	uint8_t byte0 = RXCabinsBuffer[5*cabinNumber+0];
	uint8_t byte1 = RXCabinsBuffer[5*cabinNumber+1];
	uint8_t byte2 = RXCabinsBuffer[5*cabinNumber+2];
	uint8_t byte3 = RXCabinsBuffer[5*cabinNumber+3];
	uint8_t byte4 = RXCabinsBuffer[5*cabinNumber+4];

	sum ^= byte0;
	sum ^= byte1;
	sum ^= byte2;
	sum ^= byte3;
	sum ^= byte4;

	cabin.distance1 = (byte1 << 6) | ((byte0 & B11111100) >> 2);
	cabin.distance2 = (byte3 << 6) | ((byte2 & B11111100) >> 2);

	// A la base, cette valeur est donnée en Q3 sur 6 bits (5 bits de données plus le bit de signe).
	// Je la mets directement en Q5 pour donner son sens au bit de signe dans un entier 8 bits signé.

	int8_t deltaAngle1Q5 = ((byte0 & B00000011) << 6) | ((byte4 & 0x0F) << 2);
	int8_t deltaAngle2Q5 = ((byte2 & B00000011) << 6) | ((byte4 & 0xF0) >> 2);

	cabin.deltaAngle1 = deltaAngle1Q5 / 32;
	cabin.deltaAngle2 = deltaAngle2Q5 / 32;
}

float angleDiff(float angle1, float angle2) {
	if (angle1 <= angle2) return (angle2 - angle1);
	return (360 + angle2 - angle1);
}

float computeAngle(uint8_t k, float angle1, float angle2, float delta) {
	currentAngle = angle1 - delta + (angleDiff(angle1, angle2) * k / 32);
}

void pushScan(uint16_t distance) {
	if(lastAngle > currentAngle) {
		Serial.print("Revolution. Top index : ");
		Serial.println(scanIndex);
		scanIndexTop = scanIndex;
		scanIndex = 0;
	}
	lastAngle = currentAngle;

	scanPtr = &scans[scanIndex];
	scanIndex++;
	if(scanIndex >= SAMPLE_BY_REVOLUTION) {
		Serial.println("ScanIndex overflow !!");
		scanIndexTop = SAMPLE_BY_REVOLUTION;
		scanIndex = 0;
	}

	scanPtr->distance = distance;
	scanPtr->angle = currentAngle;
	scanPtr->x = distance * cos(currentAngle * 3.14159/180);
	scanPtr->y = distance * sin(currentAngle * 3.14159/180);

	// Send debug data
	/* */ //Serial.print("SCAN:");
	/* */ //Serial.print(scanPtr->x, 2);
	/* */ //Serial.print(":");
	/* */ //Serial.println(scanPtr->y, 2);
}

void lidarRXParserLoop() {

	if(Serial1.available() < CABINS_SIZE + CABIN_HEAD_SIZE) {

		yield();
	}
	else {

		// Let's light the led while lidar reading and computing
		digitalWrite(LED_BUILTIN, HIGH);		

		// read cabins
		bytesRead = Serial1.readBytes(RXCabinsBuffer, CABINS_SIZE);
		if(bytesRead != CABINS_SIZE) stopAll("Timeout while reading cabin");
		
		// read next head needed by angle computation
		bytesRead = Serial1.readBytes(RXHeadBuffer, CABIN_HEAD_SIZE);
		if(bytesRead != CABIN_HEAD_SIZE) stopAll("Timeout while reading cabin head");
		
		// preserve current values from head
		startAngle = head.startAngle;
		checkSum = head.checkSum;
		sum = head.sum;

		// parse next header
		parseHead();
		if(head.sync1 != 0xA || head.sync2 != 0x5) stopAll("First two bytes of head should be a new packet");
		// Now head contains next header values.
		
		// Let's ignore previous cabins if start flag is reset
		if(head.startFlag == false) {

			for(uint8_t cabinNumber = 0; cabinNumber++; cabinNumber < 16) {

				parseCabin(cabinNumber);

				computeAngle(cabinNumber*2, startAngle, head.startAngle, cabin.deltaAngle1);
				pushScan(cabin.distance1);

				computeAngle(cabinNumber*2+1, startAngle, head.startAngle, cabin.deltaAngle2);
				pushScan(cabin.distance2);
			}

			// test checkSum
			if(sum != checkSum) stopAll("Wrong checkSum");
		}

		digitalWrite(LED_BUILTIN, LOW);
	}
}

void loop() {
	// debug "loop" toutes le secondes
	if(timestamp == 0) timestamp = millis();

	if(timestamp + 1000 < millis()) {
		
		timestamp = millis();

		Serial.println("=================== loop ===================");

		display.clearDisplay();

		for(uint16_t i = 0; i<scanIndexTop; i+=4) {

			display.drawPixel(map(scans[i].x, -8000, 8000, 64, 0)+64, map(scans[i].y, -8000, 8000, 0, 64), WHITE);
		}

		display.display();
	}

	yield();
}

