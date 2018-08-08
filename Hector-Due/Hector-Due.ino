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
#define HEALTH_SIZE 3

uint8_t RXResponseHeadBuffer[RESPONSE_HEAD_SIZE];
uint8_t RXCabinHeadBuffer[CABIN_HEAD_SIZE];
uint8_t RXCabinPacketBuffer[CABINS_SIZE];
uint8_t RXHealthBuffer[HEALTH_SIZE];

const uint8_t requestGetHealth[2] = {0xA5, 0x52};
const uint8_t requestStop[2] = {0xA5, 0x25};
const uint8_t requestExpressScan[9] = {0xA5, 0x82, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
const uint8_t responseHeadExpressScan[RESPONSE_HEAD_SIZE] = {0xA5, 0x5A, 0x54, 0x00, 0x00, 0x40, 0x82};
const uint8_t responseHeadGetHealth[RESPONSE_HEAD_SIZE] = {0xA5, 0x5A, 0x03, 0x00, 0x00, 0x00, 0x06};

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


/*
// Display
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define OLED_RESET 3
#define LINES 4

Adafruit_SSD1306 display(OLED_RESET);

//uint32_t timestamp = 0;
*/

void getHealth() {
	// Send get health
	Serial1.write(requestGetHealth, 2);

	// read head
	if(Serial1.readBytes(RXResponseHeadBuffer, RESPONSE_HEAD_SIZE) != RESPONSE_HEAD_SIZE) {
		stopAll("Timeout while reading get health response head");
	}

	// test response head value
	for(uint8_t i = 0; i < RESPONSE_HEAD_SIZE; i++) {
		if(responseHeadGetHealth[i] != RXResponseHeadBuffer[i]) {
			stopAll("Wrong get health response head");
		}
	}

	// read body
	if(Serial1.readBytes(RXHealthBuffer, HEALTH_SIZE) != HEALTH_SIZE){
		stopAll("Timeout while reading get health response head");
	}
	uint8_t healthStatus = RXHealthBuffer[0];
	//uint16_t healthCode = (RXHealthBuffer[2] << 8) | RXHealthBuffer[1];
	//Serial.print("Health status : ");
	//Serial.println(healthStatus);
	//Serial.print("Health err code : ");
	//Serial.println(healthCode);
	if(healthStatus != 0) {
		stopAll("Health problem");
	}

	delay(10);
}

void stopAll(String message) {
	// Arrête le moteur du lidar
	pwm_pin6.stop();
	// arrête le scan du lidar
	Serial1.write(requestStop, 2);

	// print debug message
	Serial.println(message);

	//display.clearDisplay();
	//display.display();

	while(1);
}

void setup() {
/*
	// Display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.setTextSize(2);
	display.println("Hector");
	display.display();
*/

	// Pour alerter
	pinMode(LED_BUILTIN, OUTPUT);

	// Serial : 0 (RX) and 1 (TX) => Debug USB
	Serial.begin(115200);

	// Serial 1: 19 (RX) and 18 (TX) => RP Lidar
	Serial1.begin(115200);
	while(!Serial1);
	Serial1.setTimeout(3000);

	// Start PWM RP Lidar rotor
	pwm_pin6.start(PWM_PERIOD_PIN_6,PWM_DUTY_PIN_6);

	// blink for 10 sec
	for(uint8_t i=0; i<10; i++) {
		digitalWrite(LED_BUILTIN, HIGH);
		delay(100);
		digitalWrite(LED_BUILTIN, LOW);
		delay(900);
	}

	getHealth();

	// Send express scan header
	Serial1.write(requestExpressScan, 9);
	
	// read response
	// test number of bytes read
	if(Serial1.readBytes(RXResponseHeadBuffer, RESPONSE_HEAD_SIZE) != RESPONSE_HEAD_SIZE) {
		stopAll("Timeout while reading express scan response head");
	}
	// test response head value
	for(uint8_t i = 0; i < RESPONSE_HEAD_SIZE; i++) {
		if(responseHeadExpressScan[i] != RXResponseHeadBuffer[i]) {
			stopAll("Wrong express scan response head");
		}
	}

	/* */ //Serial.println("");
	// read 1st cabin head
	if(Serial1.readBytes(RXCabinHeadBuffer, CABIN_HEAD_SIZE) != CABIN_HEAD_SIZE) {
		stopAll("Timeout while reading cabin head");
	}

	parseHead();
	
	if(head.sync1 != 0xA || head.sync2 != 0x5) {
		stopAll("First two bytes should be 0xA 0x5");
	}
}

void parseHead() {
	uint8_t byte0 = RXCabinHeadBuffer[0];
	uint8_t byte1 = RXCabinHeadBuffer[1];
	uint8_t byte2 = RXCabinHeadBuffer[2];
	uint8_t byte3 = RXCabinHeadBuffer[3];

	head.sync1 = (byte0 & 0xF0) >> 4;
	head.sync2 = (byte1 & 0xF0) >> 4;
	head.checkSum = ((byte1 & 0x0F) << 4) | (byte0 & 0x0F);
	head.startAngle = (((byte3 & B01111111) << 8) | byte2) / 64;
	head.startFlag = !!byte3 & B10000000;
	head.sum = byte2 ^ byte3;
}

void parseCabin(uint8_t cabinNumber) {
	uint8_t byte0 = RXCabinPacketBuffer[5*cabinNumber+0];
	uint8_t byte1 = RXCabinPacketBuffer[5*cabinNumber+1];
	uint8_t byte2 = RXCabinPacketBuffer[5*cabinNumber+2];
	uint8_t byte3 = RXCabinPacketBuffer[5*cabinNumber+3];
	uint8_t byte4 = RXCabinPacketBuffer[5*cabinNumber+4];

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
	if (angle1 <= angle2) {
		return (angle2 - angle1);
	}
	return (360 + angle2 - angle1);
}

float computeAngle(uint8_t k, float angle1, float angle2, float delta) {
	currentAngle = angle1 - delta + (angleDiff(angle1, angle2) * k / 32);
}

void pushScan(uint16_t distance) {
	if(lastAngle - currentAngle > 300) {
		//Serial.print("Revolution. Top index : ");
		//Serial.println(scanIndex);
		scanIndexTop = scanIndex;
		scanIndex = 0;
		//display.clearDisplay();
	}
	lastAngle = currentAngle;

	scanPtr = &scans[scanIndex];
	scanIndex++;
	if(scanIndex >= SAMPLE_BY_REVOLUTION) {
		Serial.println("ScanIndex overflow !!");
		scanIndexTop = SAMPLE_BY_REVOLUTION;
		scanIndex = SAMPLE_BY_REVOLUTION-1;
	}

	scanPtr->distance = distance;
	scanPtr->angle = currentAngle;
	scanPtr->x = distance * cos(currentAngle * 3.14159/180);
	scanPtr->y = distance * sin(currentAngle * 3.14159/180);

	// Send debug data
	/* */ //Serial.print("SCAN:");
	/* */ //Serial.print(scanPtr->x, DEC);
	/* */ //Serial.print(":");
	/* */ //Serial.println(scanPtr->y, DEC);

	//display.drawPixel(map(scanPtr->x, -8000, 8000, 0, 63)+64, map(scanPtr->y, -8000, 8000, 0, 63), WHITE);
	//display.display();
}

void lidarRXParserLoop() {

	if(Serial1.available() < CABINS_SIZE + CABIN_HEAD_SIZE) {
		return;
	}

	// read cabins
	if(Serial1.readBytes(RXCabinPacketBuffer, CABINS_SIZE) != CABINS_SIZE) {
		stopAll("Timeout while reading cabin");
	}
	// read next head needed by angle computation
	if(Serial1.readBytes(RXCabinHeadBuffer, CABIN_HEAD_SIZE) != CABIN_HEAD_SIZE) {
		stopAll("Timeout while reading cabin head");
	}

	// preserve current values from head
	startAngle = head.startAngle;
	checkSum = head.checkSum;
	sum = head.sum;

	// parse next header
	parseHead();
	// Now head contains next header values.

	if(head.sync1 != 0xA || head.sync2 != 0x5) {
		stopAll("First two bytes of head should be a new packet");
	}
	
	// Let's ignore previous cabins if start flag is reset
	//if(head.startFlag == true) {
	//	scanIndexTop = 0;
	//	scanIndex = 0;
	//}
	//else {
		for(uint8_t cabinNumber = 0; cabinNumber < 16; cabinNumber++) {

			parseCabin(cabinNumber);

			computeAngle(cabinNumber*2, startAngle, head.startAngle, cabin.deltaAngle1);
			pushScan(cabin.distance1);

			computeAngle(cabinNumber*2+1, startAngle, head.startAngle, cabin.deltaAngle2);
			pushScan(cabin.distance2);
		}

		if(sum != checkSum) {
			stopAll("Wrong checkSum");
		}
	//}
}

void loop() {

	Serial.println(Serial1.available());

	lidarRXParserLoop();

/*
	// debug "loop" toutes le secondes
	if(timestamp == 0) timestamp = millis();

	if(timestamp + 1000 < millis()) {
		
		timestamp = millis();

		Serial.println("=================== loop ===================");

		display.clearDisplay();

		for(uint16_t i = 0; i < scanIndexTop; i+=16) {

			display.drawPixel(map(scans[i].x, -8000, 8000, 0, 63)+64, map(scans[i].y, -8000, 8000, 0, 63), WHITE);
		}

		display.display();
	}
*/
}

