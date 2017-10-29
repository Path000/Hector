#ifndef Sensors_h
#define Sensors_h

#include <Wire.h>
#include <VL53L0X.h>
// https://github.com/pololu/vl53l0x-arduino/blob/master/examples/Single/Single.ino

#define PIN_XSHUT_A 2
#define PIN_XSHUT_B 4
#define PIN_XSHUT_C 3
#define PIN_XSHUT_D 5

#define ADR_SENSOR_A 51
#define ADR_SENSOR_B 52
#define ADR_SENSOR_C 53
#define ADR_SENSOR_D 54

class Sensors {
  private:
    void prepareSensor(byte pinXshut);
    void initSensor(byte pinXshut, VL53L0X *sensor, uint8_t adr);
    void scanI2CAdresses();
    uint16_t readSensor(VL53L0X *sensor);
    VL53L0X sensorA;
    VL53L0X sensorB;
    VL53L0X sensorC;
    VL53L0X sensorD;

  public:
    Sensors();
    void init();
    uint16_t readRangeA();
    uint16_t readRangeB();
    uint16_t readRangeC();
    uint16_t readRangeD();
};

#endif
