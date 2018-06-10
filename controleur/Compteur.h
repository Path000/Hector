#ifndef Compteur_h
#define Compteur_h

#include <Arduino.h>

#define SEPARATOR ':'

#define SEQUENCE_MAX_SIZE 30000

struct SpeedSampleType {
  boolean newSampleAvailable;
  long speed1; // 4 bytes
  long speed2; // 4 bytes
  unsigned int deltaSequence; // 2 bytes
};

class Compteur {
  public:
    void init(Stream* port);
    SpeedSampleType readIfAvailable(); // read and returns true if a sample of speed is ready to be used
    void resetSequence(); // used when a new move starts. It sync sequence sent by nano
  protected:
  private:
    Stream* _port;
    unsigned int _predictedSequence;
    boolean _firstSample;
};


#endif


