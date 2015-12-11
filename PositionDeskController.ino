
#include "PositionDeskController.h"

#include <limits>
#include <cmath>


PositionDeskControllerParams::PositionDeskControllerParams() {}

PositionDeskControllerParams::PositionDeskControllerParams(const char& upPin, const char& downPin)
    : HeightDeskControllerParams(upPin, downPin) {}

PositionDeskControllerParams::PositionDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed)
    : HeightDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed) {}

PositionDeskControllerParams::PositionDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
  const PositionMap& positions)
    : HeightDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed),
      positions(positions) {
  // TODO validate min/max
  setRevPositions();
}

Position PositionDeskControllerParams::getPosition(const String& name) const {
  PositionIterator it = positions.find(name);
  if (it != positions.end()) {
    return *it;
  } else {
    // empty / fail-name
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
}

Position PositionDeskControllerParams::getPosition(const double& height) const {
  RevPositionIterator it = revPositions.find(height);
  if (it != revPositions.end()) {
    return std::make_pair(it->second, it->first);
  } else {
    // empty / fail-height
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
}

Position PositionDeskControllerParams::getHigherPosition(const String& name) const {
  RevPositionIterator it = revPositions.find(getPosition(name).second);
  if (it == revPositions.end()) {
    // empty / fail-name
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  
  it++;
  if (it == revPositions.end()) {
    // already highest, return same
    it--;
  }
  return std::make_pair(it->second, it->first);
}

Position PositionDeskControllerParams::getLowerPosition(const String& name) const {
  RevPositionIterator it = revPositions.find(getPosition(name).second);
  if (it == revPositions.end()) {
    // empty / fail-name
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  if (it != revPositions.begin()) {
    // lower, if not already lowest
    it--;
  }
  return std::make_pair(it->second, it->first);
}

Position PositionDeskControllerParams::getHigherPosition(const double& height) const {
  RevPositionIterator itLower = revPositions.lower_bound(height);
  RevPositionIterator itUpper = revPositions.upper_bound(height);
  
  if (itLower == revPositions.end()) {
    // empty / higher than highest
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  if (itUpper == revPositions.end()) {
    // already highest, return same
    return std::make_pair(itLower->second, itLower->first);
  } else {
    return std::make_pair(itUpper->second, itUpper->first);
  }
}

Position PositionDeskControllerParams::getLowerPosition(const double& height) const {
  RevPositionIterator itLower = revPositions.lower_bound(height);
  RevPositionIterator itUpper = revPositions.upper_bound(height);
  
  if (itUpper == revPositions.begin()) {
    // empty / lower than lowest
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  if (itLower != revPositions.begin()) {
    // lower, if not already lowest
    itLower--;
  }
  return std::make_pair(itLower->second, itLower->first);
}

Position PositionDeskControllerParams::getHighestPosition() const {
  RevPositionIterator it = revPositions.end();
  if (it == revPositions.begin()) {
    // empty
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  it--;
  return std::make_pair(it->second, it->first);
}

Position PositionDeskControllerParams::getLowestPosition() const {
  RevPositionIterator it = revPositions.begin();
  if (it == revPositions.end()) {
    // empty
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  return std::make_pair(it->second, it->first);
}

void PositionDeskControllerParams::insertPosition(const String& name, const double& height) {
  // TODO validate min/max
  positions[name]      = height;
  revPositions[height] = name;
}

void PositionDeskControllerParams::erasePosition(const String& name) {
  const double& height = positions[name];
  revPositions.erase(height);
  positions.erase(name);
}

void PositionDeskControllerParams::setRevPositions() {
  revPositions.clear();
  
  for (PositionIterator it = positions.begin(); it != positions.end(); it++) {
    revPositions[it->second] = it->first;
  }
}


PositionDeskController::PositionDeskController(const PositionDeskControllerParams& params, const double& initialHeight)
    : HeightDeskController(params, initialHeight),
      params(params) {}

PositionDeskController::PositionDeskController(const PositionDeskControllerParams& params, const double& initialHeight, const double& initialTargetHeight)
    : HeightDeskController(params, initialHeight, initialTargetHeight),
      params(params) {}

String PositionDeskController::getTargetPosition() const {
  return params.getPosition(getTargetHeight()).first;
}

String PositionDeskController::getCurrentPosition() const {
  return params.getPosition(getCurrentHeight()).first;
}

boolean PositionDeskController::isAtTargetPosition() const {
  return isAtTargetHeight() && getTargetPosition() != String();
}

void PositionDeskController::setPosition(const String& newPositionName) {
  const Position newPosition = params.getPosition(newPositionName);
  setHeightImpl(newPosition);
}

void PositionDeskController::raisePosition() {
  const Position newPosition = params.getHigherPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setHeightImpl(newPosition);
}

void PositionDeskController::lowerPosition() {
  const Position newPosition = params.getLowerPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setHeightImpl(newPosition);
}

void PositionDeskController::setHighestPosition() {
  setHeightImpl(params.getHighestPosition());
}

void PositionDeskController::setLowestPosition() {
  setHeightImpl(params.getLowestPosition());
}

void PositionDeskController::setHeightImpl(const Position& newPosition) {
  if (!isnan(newPosition.second)) {
    HeightDeskController::setHeight(newPosition.second);
  }
}

