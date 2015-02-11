
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

double PositionDeskControllerParams::getPositionHeight(const String& name) const {
  position_map::const_iterator it = positions.find(name);
  if (it != positions.end()) {
    return it->second;
  } else {
    return std::numeric_limits<double>::quiet_NaN();
  }
}

String PositionDeskControllerParams::getPositionName(const double& height) const {
  rev_position_map::const_iterator it = revPositions.find(height);
  if (it != revPositions.end()) {
    return it->second;
  } else {
    return String();
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
  return params.getPositionName(getTargetHeight());
}

String PositionDeskController::getCurrentPosition() const {
  return params.getPositionName(getCurrentHeight());
}

boolean PositionDeskController::isAtTargetPosition() const {
  return isAtTargetHeight() && getTargetPosition() != String();
}

void PositionDeskController::setPosition(const String& newPositionName) {
  const double newHeight = params.getPositionHeight(newPositionName);
  if (!isnan(newHeight)) {
    setHeight(newHeight);
  }
}

