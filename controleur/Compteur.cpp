#include "Compteur.h"

void Compteur::init(Stream* port) {
  _port = port;
  _firstSample = true;
  _stringComplete = false;
  _sequence = 0;
  _speed1 = 0L;
  _speed2 = 0L;
}

boolean Compteur::read() {
  String rawData = "";
  while (_port->available() && !_stringComplete) {
    char inChar = (char)_port->read();
    if (inChar == '\n') {
      _stringComplete = true;
    }
    else {
      rawData += inChar;
    }
  }

  if(_stringComplete) {
    _predictedSequence = _sequence + 1;
    if(_predictedSequence >= SEQUENCE_MAX_SIZE) { // On gère manuellement le débordement
      _predictedSequence = 0;
    }

    _parseRawData(&rawData);

    if (_firstSample) {
      _firstSample = false;
      _deltaSequence = 0;
      _predictedSequence = _sequence;
    }
    else {
      _deltaSequence = _sequence - _predictedSequence;
    }
  }
  return _stringComplete;
}

long Compteur::getSpeed1() {
  return _speed1;
}

long Compteur::getSpeed2() {
  return _speed2;
}

boolean Compteur::hasLostSample() {
  return (_deltaSequence==0)?false:true;
}

int Compteur::getNumberOfSampleLost() {
  return _deltaSequence;
}

void Compteur::resetSequence() {
  _firstSample = true;
}

void Compteur::ready() {
  // ready for a new read
  _stringComplete = false;
}

void Compteur::_parseRawData(String* rawData) {
  int dataLength = rawData->length();

  int separator1Position = rawData->indexOf(SEPARATOR);
  if (separator1Position == -1) return; // TODO log it
  if (separator1Position + 1 >= dataLength) return; // TODO log it

  int separator2Position = rawData->indexOf(SEPARATOR, separator1Position+1);
  if (separator2Position == -1) return; // TODO log it
  if (separator2Position + 1 >= dataLength) return; // TODO log it

  
  String sequenceAsString = rawData->substring(0, separator1Position);
  _sequence = sequenceAsString.toInt();

  String speed1AsString = rawData->substring(separator1Position + 1, separator2Position);
  _speed1 = speed1AsString.toInt();

  String speed2AsString = rawData->substring(separator2Position + 1, dataLength);
  _speed2 = speed2AsString.toInt();
}
