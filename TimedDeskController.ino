
#include "TimedDeskController.h"


TimedDeskController::TimedDeskController(const BaseDeskControllerParams& params)
    : BaseDeskController(params) {}

double TimedDeskController::getCurrentTimeDiff() const {
  if (isDriving()) {
    return (millis() - startDrivingTime) / double(1000);
  } else {
    return (stopDrivingTime - startDrivingTime) / double(1000);
  }
}

void TimedDeskController::setDriveTimeInterval(const double& newTimeInterval, const DeskDrivingDirection& direction) {
  targetTimeInterval = std::max(newTimeInterval, 0.0);
  
  if (shouldStop()) {
    stopDrive();
  } else {
    setDrivingDirection(direction);
  }
}

boolean TimedDeskController::update() {
  if (isDriving() && shouldStop()) {
    stopDrive();
    return true;
  } else {
    return false;
  }
}

void TimedDeskController::setDrivingDirection(const DeskDrivingDirection& newDrivingDirection) {
  if (newDrivingDirection != NONE && !isEnabled()) {
    return;
  }
  
  const DeskDrivingDirection drivingDirection = getDrivingDirection();
  
  switch (newDrivingDirection) {
  case UP:
    if (drivingDirection != UP) {
      startDrivingTime = millis();
    }
    break;
    
  case NONE:
    if (drivingDirection != NONE) {
      stopDrivingTime = millis();
    }
    break;
    
  case DOWN:
    if (drivingDirection != DOWN) {
      startDrivingTime = millis();
    }
    break;
  }
  
  BaseDeskController::setDrivingDirection(newDrivingDirection);
}

boolean TimedDeskController::shouldStop() const {
  const double timeInterval = (millis() - startDrivingTime) / double(1000);
  return timeInterval >= targetTimeInterval;
}

