#include "Stop.h"

bool Stop::isReached() {
  if(analogRead(PIN_METAL_DETECTOR) == 0) {
    return true;
  }
  return false;
}

