
#include "TimedDeskController.h"


TimedDeskController::TimedDeskController(const int& upPin, const int& downPin)
    : BaseDeskController(upPin, downPin) {
}

double TimedDeskController::getCurrentTimeDiff() const {
  if (isDriving()) {
    return (millis() - startDrivingTime) / double(1000);
  } else {
    return (stopDrivingTime - startDrivingTime) / double(1000);
  }
}

void TimedDeskController::startDriveTime(const double& newTimeDiff, const DeskDrivingDirection& direction) {
  targetTimeDiff = max(newTimeDiff, 0);
  
  if (shouldStop()) {
    stopDrive();
  } else {
    startDrive(direction);
  }
}

void TimedDeskController::update() {
  if (isDriving()) {
    if (shouldStop()) {
      stopDrive();
    }
  }
}

void TimedDeskController::startDrive(const DeskDrivingDirection& direction) {
  if (!isEnabled()) {
    return;
  }
  
  const DeskDrivingDirection drivingDirection = getDrivingDirection();
  switch (direction) {
  case UP:
    if (drivingDirection != UP) {
      startDrivingTime = millis();
    }
    break;
    
  case DOWN:
    if (drivingDirection != DOWN) {
      startDrivingTime = millis();
    }
    break;
  }
  BaseDeskController::startDrive(direction);
}

void TimedDeskController::stopDrive() {
  stopDrivingTime = millis();
  BaseDeskController::stopDrive();
}

boolean TimedDeskController::shouldStop() const {
  const double timeDiff = (millis() - startDrivingTime) / double(1000);
  return timeDiff >= targetTimeDiff;
}

