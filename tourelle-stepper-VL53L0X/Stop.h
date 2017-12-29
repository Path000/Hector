#ifndef Stop_h
#define Stop_h

#include "Arduino.h"

#define PIN_METAL_DETECTOR 7

class Stop {
  public:
    bool isReached();
};

#endif
