
#include "HeightDeskController.h"


HeightDeskControllerParams::HeightDeskControllerParams() {}

HeightDeskControllerParams::HeightDeskControllerParams(const char& upPin, const char& downPin)
    : BaseDeskControllerParams(upPin, downPin) {}

HeightDeskControllerParams::HeightDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed)
    : BaseDeskControllerParams(upPin, downPin),
      minHeight(min(minHeight, maxHeight)),
      maxHeight(max(minHeight, maxHeight)),
      upSpeed(upSpeed),
      downSpeed(downSpeed) {}

double HeightDeskControllerParams::getHeightDiff() const { return maxHeight - minHeight; }

double HeightDeskControllerParams::getTimeIntervalForHeightDiff(const double& heightDiff) const {
  if (heightDiff > 0) {
    return heightDiff / upSpeed;
  } else if (heightDiff < 0) {
    return -heightDiff / downSpeed;
  } else {
    return 0;
  }
}

double HeightDeskControllerParams::getTimeIntervalForFullHeightDiff(const DeskDrivingDirection& direction) const {
  return getTimeIntervalForHeightDiff(int(direction) * getHeightDiff());
}

double HeightDeskControllerParams::getHeightDiffForTimeInterval(const double& timeInterval, const DeskDrivingDirection& direction) const {
  switch (direction) {
  case UP:
    return upSpeed * timeInterval;
    
  case DOWN:
    return -downSpeed * timeInterval;
    
  case NONE:
    return 0;
  }
}


HeightDeskController::HeightDeskController(const HeightDeskControllerParams& params, const double& initialHeight)
    : BaseDeskController(params),
      params(params) {
  lastStoppedHeight = constrain(initialHeight, params.minHeight, params.maxHeight);
  targetHeight      = lastStoppedHeight;
}

double HeightDeskController::getTargetHeight() const { return targetHeight; }

double HeightDeskController::getCurrentHeight() const {
  const double timeInterval = (millis() - startDrivingTime) / double(1000);
  const double currentHeight = lastStoppedHeight + params.getHeightDiffForTimeInterval(timeInterval, getDrivingDirection());
  return constrain(currentHeight, params.minHeight, params.maxHeight);
}

boolean HeightDeskController::isAtTargetHeight() const {
  if (targetHeight == params.minHeight || targetHeight == params.maxHeight) {
    // TODO fel ifall vi inte kör för tillfället
    const double timeInterval = (millis() - startDrivingTime) / double(1000);
    return timeInterval >= params.getTimeIntervalForFullHeightDiff(getDrivingDirection()) * 1.5;
  } else {
    return abs(targetHeight - getCurrentHeight()) <= HEIGHT_DIFF_TOLERANCE;
  }
}

void HeightDeskController::setHeight(const double& newHeight) {
  targetHeight = constrain(newHeight, params.minHeight, params.maxHeight);
  resumeDrive();
}

void HeightDeskController::resumeDrive() {
  if (isAtTargetHeight()) {
    stopDrive();
  } else {
    const double& currentHeight = getCurrentHeight();
    if (targetHeight > currentHeight) {
      startDriveUp();
    } else if (targetHeight < currentHeight) {
      startDriveDown();
    }
  }
}

boolean HeightDeskController::update() {
  if (isDriving() && isAtTargetHeight()) {
    stopDrive();
    return true;
  } else {
    return false;
  }
}

void HeightDeskController::setEnabled(const boolean& newEnabled) {
  BaseDeskController::setEnabled(newEnabled);
  
  if (/*!isEnabled() &&*/ newEnabled) {
    resumeDrive();
  }
}

void HeightDeskController::setDrivingDirection(const DeskDrivingDirection& newDrivingDirection) {
  if (newDrivingDirection != NONE && !isEnabled()) {
    return;
  }
  
  const DeskDrivingDirection drivingDirection = getDrivingDirection();
  
  switch (newDrivingDirection) {
  case UP:
    if (drivingDirection == DOWN) {
      lastStoppedHeight = getCurrentHeight();
    }
    if (drivingDirection != UP) {
      startDrivingTime = millis();
    }
    break;
    
  case NONE:
    if (drivingDirection != NONE) {
      lastStoppedHeight = getCurrentHeight();
    }
    break;
    
  case DOWN:
    if (drivingDirection == UP) {
      lastStoppedHeight = getCurrentHeight();
    }
    if (drivingDirection != DOWN) {
      startDrivingTime = millis();
    }
    break;
  }
  
  BaseDeskController::setDrivingDirection(newDrivingDirection);
}

