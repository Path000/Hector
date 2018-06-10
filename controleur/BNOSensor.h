#ifndef BNOSensor_h
#define BNOSensor_h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define SENSITIVITY 2.0
#define DEVIBRATE 5
#define BNO055_SAMPLERATE_DELAY_MS 10

class BNOSensor {
  public:
   void init();
    boolean detectImpact();
  private:
    double oldAccelX;
    double oldAccelY;
    int vibration;
    unsigned long lastSampleTime;
    Adafruit_BNO055 bno = Adafruit_BNO055();
};


#endif
