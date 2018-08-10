#include "RPLidar.h"

void RPLidar::stopScanning() {
	// arrête le scan du lidar
	Serial1.write(requestStop, 2);
}

void RPLidar::startMotor() {

	// Start PWM RP Lidar rotor
	pwm_pin6.start(PWM_PERIOD_PIN_6, PWM_DUTY_PIN_6);
}

void RPLidar::stopMotor() {

	// Arrête le moteur du lidar
	pwm_pin6.stop();
}

const char* RPLidar::startScan() {

	const char* errorMessage = getHealth();
	if(errorMessage) return errorMessage;

	// Send express scan header
	Serial1.write(requestExpressScan, 9);

	// read response
	// test number of bytes read
	if(Serial1.readBytes(RXResponseHeadBuffer, RESPONSE_HEAD_SIZE) != RESPONSE_HEAD_SIZE) {
		return "Timeout while reading express scan response head";
	}
	// test response head value
	for(uint8_t i = 0; i < RESPONSE_HEAD_SIZE; i++) {
		if(responseHeadExpressScan[i] != RXResponseHeadBuffer[i]) {
			return "Wrong express scan response head";
		}
	}

	/* */ //Serial.println("");
	// read 1st cabin head
	if(Serial1.readBytes(RXCabinHeadBuffer, CABIN_HEAD_SIZE) != CABIN_HEAD_SIZE) {
		return "Timeout while reading cabin head";
	}

	parseHead();

	if(head.sync1 != 0xA || head.sync2 != 0x5) {
		return "First two bytes should be 0xA 0x5";
	}

	return NULL;
}

const char* RPLidar::loopScan() {

	revolution = false;

	if(Serial1.available() < CABINS_SIZE + CABIN_HEAD_SIZE) {
		return NULL;
	}

	// read cabins
	if(Serial1.readBytes(RXCabinPacketBuffer, CABINS_SIZE) != CABINS_SIZE) {
		return "Timeout while reading cabin";
	}
	// read next head needed by angle computation
	if(Serial1.readBytes(RXCabinHeadBuffer, CABIN_HEAD_SIZE) != CABIN_HEAD_SIZE) {
		return "Timeout while reading cabin head";
	}

	// preserve current values from head
	startAngle = head.startAngle;
	checkSum = head.checkSum;
	sum = head.sum;

	// parse next header
	parseHead();
	// Now head contains next header values.

	if(head.sync1 != 0xA || head.sync2 != 0x5) {
		return "First two bytes of head should be a new packet";
	}

	for(uint8_t cabinNumber = 0; cabinNumber < 16; cabinNumber++) {

		parseCabin(cabinNumber);

		computeAngle(cabinNumber*2, startAngle, head.startAngle, cabin.deltaAngle1);
		const char* errorMessage = pushScan(cabin.distance1);
		if(errorMessage) return errorMessage;

		computeAngle(cabinNumber*2+1, startAngle, head.startAngle, cabin.deltaAngle2);
		errorMessage = pushScan(cabin.distance2);
		if(errorMessage) return errorMessage;
	}

	if(sum != checkSum) {
		return "Wrong checkSum";
	}
}

const char* RPLidar::getHealth() {

	// Send get health
	Serial1.write(requestGetHealth, 2);

	// read head
	if(Serial1.readBytes(RXResponseHeadBuffer, RESPONSE_HEAD_SIZE) != RESPONSE_HEAD_SIZE) {
		return "Timeout while reading get health response head";
	}

	// test response head value
	for(uint8_t i = 0; i < RESPONSE_HEAD_SIZE; i++) {
		if(responseHeadGetHealth[i] != RXResponseHeadBuffer[i]) {
			return "Wrong get health response head";
		}
	}

	// read body
	if(Serial1.readBytes(RXHealthBuffer, HEALTH_SIZE) != HEALTH_SIZE){
		return "Timeout while reading get health response head";
	}

	uint8_t healthStatus = RXHealthBuffer[0];
	//uint16_t healthCode = (RXHealthBuffer[2] << 8) | RXHealthBuffer[1];
	//Serial.print("Health status : ");
	//Serial.println(healthStatus);
	//Serial.print("Health err code : ");
	//Serial.println(healthCode);
	if(healthStatus != 0) {
		return "Health problem";
	}

	delay(10);

	return NULL;
}

void RPLidar::parseHead() {

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

void RPLidar::parseCabin(uint8_t cabinNumber) {

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

float RPLidar::angleDiff(float angle1, float angle2) {

	if (angle1 <= angle2) {
		return (angle2 - angle1);
	}
	return (360 + angle2 - angle1);
}

float RPLidar::computeAngle(uint8_t k, float angle1, float angle2, float delta) {

	currentAngle = angle1 - delta + (angleDiff(angle1, angle2) * k / 32);
}

const char* RPLidar::pushScan(uint16_t distance) {

	if(lastAngle - currentAngle > 300) {
		scanIndexTop = scanIndex;
		scanIndex = 0;
		revolution = true;
	}
	lastAngle = currentAngle;

	scanPtr = &scans[scanIndex];
	scanIndex++;
	if(scanIndex >= SAMPLE_BY_REVOLUTION) {
		return "ScanIndex overflow !!";
	}

	scanPtr->distance = distance;
	scanPtr->angle = currentAngle;
	scanPtr->x = distance * cos(currentAngle * 3.14159/180);
	scanPtr->y = distance * sin(currentAngle * 3.14159/180);

}
