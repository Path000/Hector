#include "Command.h"

void Command::init(Stream* port) {
	_port = port;
}

ParsedCommandType* Command::readIfAvailable() {

	_data.newCommandAvailable = false;

	if(_port->available() < 1) return &_data;
	
	String inputString = _port->readStringUntil('\n');

	_data.newCommandAvailable = true;

	int dataLength = inputString.length();
	int firstSep = inputString.indexOf(COMMAND_SEPARATOR);
	if(firstSep == -1) {
		_data.cmd = inputString;
		return &_data;
	}
	if(firstSep + 1 >= dataLength) return &_data;
	_data.cmd = inputString.substring(0,firstSep);
	// get args
	unsigned int index = 0;
	while(firstSep + 1 < dataLength && index < MAX_ARGS) {
		int secondSep = inputString.indexOf(COMMAND_SEPARATOR, firstSep+1);
		// pour le dernier argument
		if(secondSep == -1) {
		  secondSep = dataLength;
		}
		String arg = inputString.substring(firstSep+1, secondSep);
		firstSep = secondSep;
		_data.arrayArgs[index] = arg.toInt();
		index++;
	}
	
	return &_data;
}

void Command::send(String message) {
	_port->println(message);
}