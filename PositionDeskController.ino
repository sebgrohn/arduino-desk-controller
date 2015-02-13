
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
  const position_map& positions)
    : HeightDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed),
      positions(positions) {
  // TODO validate min/max
  setRevPositions();
}

PositionDeskControllerParams::position PositionDeskControllerParams::getPosition(const String& name) const {
  position_map::const_iterator it = positions.find(name);
  if (it != positions.end()) {
    return *it;
  } else {
    // empty / fail-name
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
}

PositionDeskControllerParams::position PositionDeskControllerParams::getPosition(const double& height) const {
  rev_position_map::const_iterator it = revPositions.find(height);
  if (it != revPositions.end()) {
    return std::make_pair(it->second, it->first);
  } else {
    // empty / fail-height
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
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
  
  for (position_map::const_iterator it = positions.begin(); it != positions.end(); it++) {
    revPositions[it->second] = it->first;
  }
}


PositionDeskController::PositionDeskController(const PositionDeskControllerParams& params, const double& initialHeight)
    : HeightDeskController(params, initialHeight),
      params(params) {
}

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
  const PositionDeskControllerParams::position newPosition = params.getPosition(newPositionName);
  if (!isnan(newPosition.second)) {
    setHeight(newPosition.second);
  }
}

