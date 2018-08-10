#ifndef RPLidar_h
#define RPLidar_h

#include <Arduino.h>
#include <stdint.h>

#define RESPONSE_HEAD_SIZE 7
#define CABIN_HEAD_SIZE 4
#define CABINS_SIZE 80
#define HEALTH_SIZE 3

// Store 1 full revolution
// 60% PWM => 0.9° resolution at 10Hz => 400 scan / turn (refering doc)
#define SAMPLE_BY_REVOLUTION 500

struct HeadStruct {
	int8_t sync1; // New packet marker
	int8_t sync2;
	float startAngle; // current packet's startAngle and next's one are needed for current cabins
	uint8_t checkSum; // current packet's checksum
	uint8_t sum; // current head's sum
	bool startFlag; // if true, current packet should be ignored
};

struct CabinStruct {
	uint16_t distance1;
	uint16_t distance2;
	float deltaAngle1;
	float deltaAngle2;
};

struct ScanStruct {
	uint16_t distance;
	float angle;
	int16_t x;
	int16_t y;
};

#include "pwm_lib.h"
using namespace arduino_due::pwm_lib;

// pour le lidar -> 25kHz et 60% duty
// PWML7_PC24  // PWM_CH7 -> Le pin est le seul sur ce channel -> pin6
#define PWM_PERIOD_PIN_6 4000 // hundredth of usecs (1e-8 secs)
#define PWM_DUTY_PIN_6 2400 // hundredth of usecs (1e-8 secs)

class RPLidar {
	public :
		// returns error message if error
		// returns NULL if no error
		const char* startScan();
		const char* loopScan();
		void stopScanning();
		void startMotor();
		void stopMotor();
		// indicates if a complete turn is done
		bool revolution = false;
	private :
		// RX buffer
		uint8_t RXResponseHeadBuffer[RESPONSE_HEAD_SIZE];
		uint8_t RXCabinHeadBuffer[CABIN_HEAD_SIZE];
		uint8_t RXCabinPacketBuffer[CABINS_SIZE];
		uint8_t RXHealthBuffer[HEALTH_SIZE];
		// requests
		const uint8_t requestGetHealth[2] = {0xA5, 0x52};
		const uint8_t requestExpressScan[9] = {0xA5, 0x82, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
		const uint8_t requestStop[2] = {0xA5, 0x25};
		// response heads
		const uint8_t responseHeadExpressScan[RESPONSE_HEAD_SIZE] = {0xA5, 0x5A, 0x54, 0x00, 0x00, 0x40, 0x82};
		const uint8_t responseHeadGetHealth[RESPONSE_HEAD_SIZE] = {0xA5, 0x5A, 0x03, 0x00, 0x00, 0x00, 0x06};

		// current parsed head
		HeadStruct head;
		// current parsed cabin
		CabinStruct cabin;
		// preserved head values
		float startAngle;
		uint8_t checkSum;
		uint8_t sum;

		ScanStruct scans[SAMPLE_BY_REVOLUTION];
		ScanStruct* scanPtr;
		uint16_t scanIndex = 0;
		uint16_t scanIndexTop = 0;
		float lastAngle = 0;
		float currentAngle = 0;

		const char* pushScan(uint16_t distance);
		float computeAngle(uint8_t k, float angle1, float angle2, float delta);
		float angleDiff(float angle1, float angle2);
		void parseCabin(uint8_t cabinNumber);
		void parseHead();
	  	const char* getHealth();

		// pour le driver moteur
		pwm<pwm_pin::PWML7_PC24> pwm_pin6;
};

#endif