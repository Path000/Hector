#ifndef Command_h
#define Command_h

#include <Arduino.h>

#define COMMAND_SEPARATOR ':'
#define MAX_ARGS 2

typedef struct {
	String cmd;
	String arrayArgs[MAX_ARGS];
} ParsedCommand;

class Command {
	public:
		void init(Stream* port);
		boolean read();
		ParsedCommand get();
		void send(String message);
	private:
		Stream* _port;
		ParsedCommand _parse();
		String _inputString;
		boolean _stringComplete;
};


#endif
