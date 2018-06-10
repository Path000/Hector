#ifndef Compteur_h
#define Compteur_h

#include <Arduino.h>

#define SEPARATOR ':'

#define SEQUENCE_MAX_SIZE 30000

class Compteur {
  public:
    void init(Stream* port);
    boolean read(); // read and returns true if a sample of speed is ready to be used
    long getSpeed1();
    long getSpeed2();
    boolean hasLostSample(); // returns true if a sample is lost, based on sequence number
    int getNumberOfSampleLost(); // returns number of lost sample, based on sequence number
    void ready(); // Data is read and ready to read next
    void resetSequence(); // used when a new move starts. It sync sequence sent by nano
  protected:
  private:
    void _parseRawData(String* rawData);
    Stream* _port;
    unsigned int _predictedSequence;
    boolean _firstSample;
    boolean _stringComplete;
    unsigned int _sequence;
    int _deltaSequence;
    long _speed1;
    long _speed2;
};


#endif


