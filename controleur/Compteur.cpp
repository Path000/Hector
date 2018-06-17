#include "Compteur.h"

void Compteur::init(Stream* port) {
  _port = port;
}

void Compteur::readyToRead() {
  _port->write(1);
}

SpeedSampleType* Compteur::readIfAvailable() {

  
  _sampleData.newSampleAvailable = false;

  if(_port->available() < 1) return &_sampleData;

  String rawData = _port->readStringUntil('\n');

  int dataLength = rawData.length();

  int separator1Position = rawData.indexOf(SEPARATOR);
  if (separator1Position == -1) return &_sampleData;
  if (separator1Position + 1 >= dataLength) return &_sampleData;

  String speed1AsString = rawData.substring(0, separator1Position);
  _sampleData.speed1 = speed1AsString.toInt();

  String speed2AsString = rawData.substring(separator1Position + 1, dataLength);
  _sampleData.speed2 = speed2AsString.toInt();

  _sampleData.newSampleAvailable = true;

  return &_sampleData;
}
