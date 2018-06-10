#include "Sensors.h"

Sensors::Sensors() {
  
}

void Sensors::init() {
  prepareSensor(PIN_XSHUT_A);
  prepareSensor(PIN_XSHUT_B);
  prepareSensor(PIN_XSHUT_C);
  prepareSensor(PIN_XSHUT_D);
  delay(500);
  Wire.begin();
  initSensor(PIN_XSHUT_A, &sensorA, (uint8_t)ADR_SENSOR_A);
  initSensor(PIN_XSHUT_B, &sensorB, (uint8_t)ADR_SENSOR_B);
  initSensor(PIN_XSHUT_C, &sensorC, (uint8_t)ADR_SENSOR_C);
  initSensor(PIN_XSHUT_D, &sensorD, (uint8_t)ADR_SENSOR_D);

  //scanI2CAdresses();
}

/*
void Sensors::scanI2CAdresses() {
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 1; i < 120; i++)
  {

    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
    } // end of good response
  } // end of for loop
  Serial.print ("Done. Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}
*/

/* Set XSHUT to LOW */
void Sensors::prepareSensor(byte pinXshut) {
  pinMode(pinXshut, OUTPUT);
  digitalWrite(pinXshut, LOW);
}


// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensor and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

#define LONG_RANGE


// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

//#define HIGH_SPEED
//#define HIGH_ACCURACY

/* Set sensor adress and parameters */
void Sensors::initSensor(byte pinXshut, VL53L0X *sensor, uint8_t adr) {
  pinMode(pinXshut, INPUT);
  delay(200);
  sensor->init(true);
  delay(100);
  sensor->setAddress(adr);
  sensor->setTimeout((uint16_t)500);
  
#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor->setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor->setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor->setMeasurementTimingBudget(200000);
#endif
}

uint16_t Sensors::readSensor(VL53L0X *sensor) {
  uint16_t distance = sensor->readRangeSingleMillimeters();
  if (sensor->timeoutOccurred() || distance > 8000) {
    return 0;
  }
  return distance;
}

uint16_t Sensors::readRangeA() {
  return readSensor(&sensorA);
}
uint16_t Sensors::readRangeB() {
  return readSensor(&sensorB);
}
uint16_t Sensors::readRangeC() {
  return readSensor(&sensorC);
}
uint16_t Sensors::readRangeD() {
  return readSensor(&sensorD);
}

