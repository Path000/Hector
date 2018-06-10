#include "BNOSensor.h"

void BNOSensor::init() {
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  bno.setExtCrystalUse(true);
  oldAccelX = 0;                                              // store previous accelerometer readings for comparison
  oldAccelY = 0;
  lastSampleTime = millis();
}

boolean BNOSensor::detectImpact() {
  
  if(millis() - lastSampleTime < BNO055_SAMPLERATE_DELAY_MS) return false;
  lastSampleTime = millis();
  vibration--;                                                // loop counter prevents false triggering cause by impact vibration
  if(vibration<0) vibration=0;                                // as robot vibrates due to impact
  
  if(vibration>0) return false;                                     // until vibration has subsided no further calculations required

  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  imu::Vector<3> vector = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  double accelX = vector.x();
  double accelY = vector.y();
  
  double magnitude = sqrt( sq(accelX - oldAccelX) + sq(accelY - oldAccelY) );
  
  oldAccelX = accelX;                                              // store previous accelerometer readings for comparison
  oldAccelY = accelY;

  if (magnitude > SENSITIVITY) {           // has a new impact occured
    vibration=DEVIBRATE;                                      // reset anti-vibration counter
    return true;
  }

  return false;
}
