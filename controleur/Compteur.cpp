#include "Compteur.h"

void Compteur::init(Stream* port) {
  _port = port;
  _firstSample = true;
}

SpeedSampleType Compteur::readIfAvailable() {

  SpeedSampleType sampleData;
  sampleData.newSampleAvailable = false;

  if(_port->available() < 1) return sampleData;

  String rawData = _port->readStringUntil('\n');

  int dataLength = rawData.length();

  int separator1Position = rawData.indexOf(SEPARATOR);
  if (separator1Position == -1) return sampleData; // TODO log it
  if (separator1Position + 1 >= dataLength) return sampleData; // TODO log it

  int separator2Position = rawData.indexOf(SEPARATOR, separator1Position+1);
  if (separator2Position == -1) return sampleData; // TODO log it
  if (separator2Position + 1 >= dataLength) return sampleData; // TODO log it

  String sequenceAsString = rawData.substring(0, separator1Position);
  int currentSequence = sequenceAsString.toInt();

  String speed1AsString = rawData.substring(separator1Position + 1, separator2Position);
  sampleData.speed1 = speed1AsString.toInt();

  String speed2AsString = rawData.substring(separator2Position + 1, dataLength);
  sampleData.speed2 = speed2AsString.toInt();


  if (_firstSample) {

    _firstSample = false;
    sampleData.deltaSequence = 0;
    _predictedSequence = currentSequence;
  }
  else {

    _predictedSequence += 1;
    if(_predictedSequence >= SEQUENCE_MAX_SIZE) { // On gère manuellement le débordement
      _predictedSequence = 0;
    }
    sampleData.deltaSequence = currentSequence - _predictedSequence;
    
    if(sampleData.deltaSequence != 0) _predictedSequence = currentSequence;
  }

  sampleData.newSampleAvailable = true;
  return sampleData;
}

void Compteur::resetSequence() {
  _firstSample = true;
}
