#ifndef StateScanning_h
#define StateScanning_h

#include "StateError.h"

class StateScanning : public State {
  public:
  	void setStateError(StateError* stateError);
  	State* onStart();
  	State* run();
  private:
  	StateError* _stateError;
};


#endif