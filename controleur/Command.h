#ifndef Command_h
#define Command_h

#include <Arduino.h>

#define COMMAND_SEPARATOR ':'
#define MAX_ARGS 2

struct ParsedCommandType {
	boolean newCommandAvailable;
	String cmd;
	String arrayArgs[MAX_ARGS];
};

class Command {
	public:
		void init(Stream* port);
		ParsedCommandType* readIfAvailable();
		void send(String message);
	private:
		Stream* _port;
		ParsedCommandType _data;
};


#endif
