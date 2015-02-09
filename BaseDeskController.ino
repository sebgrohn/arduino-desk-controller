
#include "BaseDeskController.h"


BaseDeskControllerParams::BaseDeskControllerParams() {}

BaseDeskControllerParams::BaseDeskControllerParams(const char& upPin, const char& downPin)
    : upPin(upPin),
      downPin(downPin) {}


BaseDeskController::BaseDeskController(const BaseDeskControllerParams& params)
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
    stopDrive();
  }
  
  enabled = newEnabled;
}

void BaseDeskController::setDrivingDirection(const DeskDrivingDirection& newDrivingDirection) {
  if (newDrivingDirection != NONE && !enabled) {
    return;
  }
  
  switch (newDrivingDirection) {
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

void BaseDeskController::startDriveUp()   { setDrivingDirection(UP); }
void BaseDeskController::startDriveDown() { setDrivingDirection(DOWN); }
void BaseDeskController::stopDrive()      { setDrivingDirection(NONE); }

