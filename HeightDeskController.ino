
#include "HeightDeskController.h"


DeskControllerParams::DeskControllerParams(const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed)
    : minHeight(min(minHeight, maxHeight)), maxHeight(max(minHeight, maxHeight)), upSpeed(upSpeed), downSpeed(downSpeed) {
}

double DeskControllerParams::getHeightDiff() const { return maxHeight - minHeight; }

double DeskControllerParams::getTimeForHeightDiff(const double& heightDiff) const {
  if (heightDiff > 0) {
    return heightDiff / upSpeed;
  } else if (heightDiff < 0) {
    return -heightDiff / downSpeed;
  } else {
    return 0;
  }
}

double DeskControllerParams::getTimeForFullHeightDiff(const DeskDrivingDirection& direction) const {
  return getTimeForHeightDiff(int(direction) * getHeightDiff());
}

double DeskControllerParams::getHeightDiffForTime(const double& timeDiff, const DeskDrivingDirection& direction) const {
  switch (direction) {
  case UP:
    return upSpeed * timeDiff;
    
  case DOWN:
    return -downSpeed * timeDiff;
    
  case NONE:
    return 0;
  }
}


HeightDeskController::HeightDeskController(const DeskControllerParams& params, const int& upPin, const int& downPin, const double& initialHeight)
    : BaseDeskController(upPin, downPin), params(params) {
  lastStoppedHeight = constrain(initialHeight, params.minHeight, params.maxHeight);
  targetHeight      = lastStoppedHeight;
}

double HeightDeskController::getTargetHeight() const { return targetHeight; }

double HeightDeskController::getCurrentHeight() const {
  const double timeDiff = (millis() - startDrivingTime) / double(1000);
  const double currentHeight = lastStoppedHeight + params.getHeightDiffForTime(timeDiff, getDrivingDirection());
  return constrain(currentHeight, params.minHeight, params.maxHeight);
}

boolean HeightDeskController::isAtTargetHeight() const {
  if (targetHeight == params.minHeight || targetHeight == params.maxHeight) {
    const double timeDiff = (millis() - startDrivingTime) / double(1000);
    return timeDiff >= params.getTimeForFullHeightDiff(getDrivingDirection()) * 1.5;
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
    // TODO fel ifall targetHeight == (params.minHeight || params.maxHeight) och vi inte kör för tillfället
    stopDrive();
  } else {
    const double& currentHeight = getCurrentHeight();
    if (targetHeight > currentHeight) {
      startDrive(UP);
    } else if (targetHeight < currentHeight) {
      startDrive(DOWN);
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

void HeightDeskController::startDrive(const DeskDrivingDirection& direction) {
  if (!isEnabled()) {
    return;
  }
  
  const DeskDrivingDirection drivingDirection = getDrivingDirection();
  switch (direction) {
  case UP:
    if (drivingDirection == DOWN) {
      lastStoppedHeight = getCurrentHeight();
    }
    
    if (drivingDirection != UP) {
      startDrivingTime = millis();
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
  BaseDeskController::startDrive(direction);
}

void HeightDeskController::stopDrive() {
  lastStoppedHeight = getCurrentHeight();
  BaseDeskController::stopDrive();
}

