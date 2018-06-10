#include "Command.h"

void Command::init(Stream* port) {
	_port = port;
	_inputString = "";
	_stringComplete = false;
}

boolean Command::read() {

	while (_port->available() && !_stringComplete) {
		char inChar = (char)_port->read();
		if (inChar == '\n') {
			_stringComplete = true;
			//_port->print("CONSOLE:");
			//_port->println(_inputString);
		} else {
			_inputString += inChar;
		}
	}
	return _stringComplete;
}

ParsedCommand Command::_parse() {

	ParsedCommand data;
	if (!_stringComplete) {
		return data;
	}

	int dataLength = _inputString.length();
	int firstSep = _inputString.indexOf(COMMAND_SEPARATOR);
	if(firstSep == -1) {
		data.cmd = _inputString;
		return data;
	}
	if(firstSep + 1 >= dataLength) return data;
	data.cmd = _inputString.substring(0,firstSep);
	// get args
	unsigned int index = 0;
	while(firstSep + 1 < dataLength && index < MAX_ARGS) {
		int secondSep = _inputString.indexOf(COMMAND_SEPARATOR, firstSep+1);
		// pour le dernier argument
		if(secondSep == -1) {
		  secondSep = dataLength;
		}
		String arg = _inputString.substring(firstSep+1, secondSep);
		firstSep = secondSep;
		data.arrayArgs[index] = arg;
		index++;
	}
	return data;
}

ParsedCommand Command::get() {

	ParsedCommand data = _parse();

	_inputString = "";
	_stringComplete = false;

	return data;
}

void Command::send(String message) {
	_port->println(message);
}