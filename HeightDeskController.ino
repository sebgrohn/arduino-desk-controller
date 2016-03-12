
#include "HeightDeskController.h"

#include <limits>
#include <cmath>


HeightDeskControllerParams::HeightDeskControllerParams() {}

HeightDeskControllerParams::HeightDeskControllerParams(const char& upPin, const char& downPin)
    : BaseDeskControllerParams(upPin, downPin) {}

HeightDeskControllerParams::HeightDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed)
    : BaseDeskControllerParams(upPin, downPin),
      minHeight(std::min(minHeight, maxHeight)),
      maxHeight(std::max(minHeight, maxHeight)),
      upSpeed(upSpeed),
      downSpeed(downSpeed) {}

double HeightDeskControllerParams::getHeightDiff() const { return maxHeight - minHeight; }

double HeightDeskControllerParams::getHeightDiffForTimeInterval(const double& timeInterval, const DeskDrivingDirection& direction) const {
  switch (direction) {
    case UP:   return upSpeed * timeInterval;
    case NONE: return 0;
    case DOWN: return -downSpeed * timeInterval;
  }
}

double HeightDeskControllerParams::getTimeIntervalForHeightDiff(const double& heightDiff) const {
  if (heightDiff > 0) {
    return heightDiff / upSpeed;
  } else if (heightDiff < 0) {
    return heightDiff / -downSpeed;
  } else {
    return 0;
  }
}

double HeightDeskControllerParams::getTimeIntervalForFullHeightDiff(const DeskDrivingDirection& direction) const {
  switch (direction) {
  case NONE:
    return std::numeric_limits<double>::infinity();
    
  default:
    return getTimeIntervalForHeightDiff(int(direction) * getHeightDiff());
  }
}


HeightDeskController::HeightDeskController(const Params& params, const double& initialHeight)
    : HeightDeskController(params, initialHeight, initialHeight) {}

HeightDeskController::HeightDeskController(const Params& params, const double& initialHeight, const double& initialTargetHeight)
    : BaseDeskController(params),
      params(params) {
  lastStoppedHeight = constrain(initialHeight,       params.minHeight, params.maxHeight);
  targetHeight      = constrain(initialTargetHeight, params.minHeight, params.maxHeight);
}

double HeightDeskController::getTargetHeight() const { return targetHeight; }

double HeightDeskController::getCurrentHeight() const {
  const double timeInterval = (millis() - startDrivingTime) / double(1000);
  const double currentHeight = lastStoppedHeight + params.getHeightDiffForTimeInterval(timeInterval, getDrivingDirection());
  return constrain(currentHeight, params.minHeight, params.maxHeight);
}

boolean HeightDeskController::isAtTargetHeight() const {
  if (targetHeight == params.minHeight || targetHeight == params.maxHeight) {
    const double timeInterval = (millis() - startDrivingTime) / double(1000);
    return timeInterval >= params.getTimeIntervalForFullHeightDiff(getDrivingDirection()) * 1.5;
  } else {
    // TODO workaround for abs converting to integer
    const double heightDiff = targetHeight - getCurrentHeight();
    return (heightDiff < 0 ? -heightDiff : heightDiff) <= HEIGHT_DIFF_TOLERANCE;
    
    //return std::abs(targetHeight - getCurrentHeight()) <= HEIGHT_DIFF_TOLERANCE;
  }
}

DeskDrivingDirection HeightDeskController::getTargetDrivingDirection() const {
  if (isAtTargetHeight()) {
    return NONE;
  } else {
    const double currentHeight = getCurrentHeight();
    if (targetHeight > currentHeight) {
      return UP;
    } else if (targetHeight < currentHeight) {
      return DOWN;
    }
    return NONE;  // will never be reached
  }
}

boolean HeightDeskController::wantToDrive() const     { return getTargetDrivingDirection() != NONE; }
boolean HeightDeskController::wantToDriveUp() const   { return getTargetDrivingDirection() == UP; }
boolean HeightDeskController::wantToDriveDown() const { return getTargetDrivingDirection() == DOWN; }

void HeightDeskController::setHeight(const double& newHeight) {
  targetHeight = constrain(newHeight, params.minHeight, params.maxHeight);
  resumeDriving();
}

void HeightDeskController::resumeDriving() {
  setDrivingDirection(getTargetDrivingDirection());
}

boolean HeightDeskController::update() {
  if (isDriving() && isAtTargetHeight()) {
    stopDrive();
    return true;
  } else {
    return false;
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

