#ifndef Compteur_h
#define Compteur_h

#include <Arduino.h>

#define SEPARATOR ':'

struct SpeedSampleType {
  boolean newSampleAvailable;
  unsigned int speed1; // 4 bytes
  unsigned int speed2; // 4 bytes
};

class Compteur {
  public:
    void init(Stream* port);
    SpeedSampleType* readIfAvailable(); // read and returns true if a sample of speed is ready to be used
  protected:
  private:
    Stream* _port;
    SpeedSampleType _sampleData;
};


#endif


