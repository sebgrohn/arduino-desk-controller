
#include "BaseDeskController.h"


BaseDeskControllerParams::BaseDeskControllerParams() {}

BaseDeskControllerParams::BaseDeskControllerParams(const char& upPin, const char& downPin)
    : upPin(upPin),
      downPin(downPin) {}


BaseDeskController::BaseDeskController(const Params& params)
    : params(params) {
  pinMode(params.upPin,   OUTPUT);
  pinMode(params.downPin, OUTPUT);
  
  enabled = true;
}

BaseDeskController::~BaseDeskController() {}

boolean BaseDeskController::isEnabled() const { return enabled; }

DeskDrivingDirection BaseDeskController::getDrivingDirection() const {return drivingDirection; }

boolean BaseDeskController::isDriving() const     { return drivingDirection != NONE; }
boolean BaseDeskController::isDrivingUp() const   { return drivingDirection == UP; }
boolean BaseDeskController::isDrivingDown() const { return drivingDirection == DOWN; }

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

void BaseDeskController::driveUp()   { setDrivingDirection(UP); }
void BaseDeskController::driveDown() { setDrivingDirection(DOWN); }
void BaseDeskController::stop()      { setDrivingDirection(NONE); }

