
#include "BaseDeskController.h"


BaseDeskControllerParams::BaseDeskControllerParams(const char& upPin, const char& downPin)
    : upPin(upPin),
      downPin(downPin) {}


BaseDeskController::BaseDeskController(const Params& params)
    : params(params) {
  pinMode(params.upPin,   OUTPUT);
  pinMode(params.downPin, OUTPUT);
  enabled = true;
}

void BaseDeskController::setEnabled(const boolean& newEnabled) {
  if (enabled && !newEnabled) {
    stop();
  }
  enabled = newEnabled;
}

void BaseDeskController::toggleEnabled() {
  if (enabled) {
    setEnabled(false);
  } else {
    setEnabled(true);
  }
}

void BaseDeskController::setDrivingDirection(const DeskDrivingDirection& newDirection) {
  if (newDirection != NONE && !enabled) {
    return;
  }
  
  switch (newDirection) {
  case UP:
    drivingDirection = UP;
    digitalWrite(params.downPin, LOW);
    digitalWrite(params.upPin,   HIGH);
    break;
    
  case NONE:
    digitalWrite(params.upPin,   LOW);
    digitalWrite(params.downPin, LOW);
    drivingDirection = NONE;
    break;
    
  case DOWN:
    drivingDirection = DOWN;
    digitalWrite(params.upPin,   LOW);
    digitalWrite(params.downPin, HIGH);
    break;
  }
}

