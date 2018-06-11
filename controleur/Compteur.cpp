#include "Compteur.h"

void Compteur::init(Stream* port) {
  _port = port;
  _firstSample = true;
}

SpeedSampleType* Compteur::readIfAvailable() {

  
  _sampleData.newSampleAvailable = false;

  if(_port->available() < 1) return &_sampleData;

  String rawData = _port->readStringUntil('\n');

  int dataLength = rawData.length();

  int separator1Position = rawData.indexOf(SEPARATOR);
  if (separator1Position == -1) return &_sampleData;
  if (separator1Position + 1 >= dataLength) return &_sampleData;

  int separator2Position = rawData.indexOf(SEPARATOR, separator1Position+1);
  if (separator2Position == -1) return &_sampleData;
  if (separator2Position + 1 >= dataLength) return &_sampleData; // TODO log it

  String sequenceAsString = rawData.substring(0, separator1Position);
  int currentSequence = sequenceAsString.toInt();

  String speed1AsString = rawData.substring(separator1Position + 1, separator2Position);
  _sampleData.speed1 = speed1AsString.toInt();

  String speed2AsString = rawData.substring(separator2Position + 1, dataLength);
  _sampleData.speed2 = speed2AsString.toInt();


  if (_firstSample) {

    _firstSample = false;
    _sampleData.deltaSequence = 0;
    _predictedSequence = currentSequence;
  }
  else {

    _predictedSequence += 1;
    if(_predictedSequence >= SEQUENCE_MAX_SIZE) { // On gère manuellement le débordement
      _predictedSequence = 0;
    }
    _sampleData.deltaSequence = currentSequence - _predictedSequence;
    
    if(_sampleData.deltaSequence != 0) _predictedSequence = currentSequence;
  }

  _sampleData.newSampleAvailable = true;
  return &_sampleData;
}

void Compteur::resetSequence() {
  _firstSample = true;
}
