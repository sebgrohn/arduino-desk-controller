
#include "PositionDeskController.h"

#include <limits>
#include <cmath>


template<size_t N>
PositionDeskControllerParams<N>::PositionDeskControllerParams() {}

template<size_t N>
PositionDeskControllerParams<N>::PositionDeskControllerParams(const char& upPin, const char& downPin)
    : HeightDeskControllerParams(upPin, downPin) {}

template<size_t N>
PositionDeskControllerParams<N>::PositionDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed)
    : HeightDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed) {}

template<size_t N>
PositionDeskControllerParams<N>::PositionDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
  const PositionMap& positions)
    : HeightDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed),
      positions(positions) {
  // TODO validate min/max
  setRevPositions();
}

template<size_t N>
Position PositionDeskControllerParams<N>::getPosition(const String& name) const {
  PositionIterator it = positions.find(name);
  if (it != positions.end()) {
    return *it;
  } else {
    // empty / fail-name
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
}

template<size_t N>
Position PositionDeskControllerParams<N>::getPosition(const double& height) const {
  RevPositionIterator it = revPositions.find(height);
  if (it != revPositions.end()) {
    return std::make_pair(it->second, it->first);
  } else {
    // empty / fail-height
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
}

template<size_t N>
Position PositionDeskControllerParams<N>::getHigherPosition(const String& name) const {
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

template<size_t N>
Position PositionDeskControllerParams<N>::getLowerPosition(const String& name) const {
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

template<size_t N>
Position PositionDeskControllerParams<N>::getHigherPosition(const double& height) const {
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

template<size_t N>
Position PositionDeskControllerParams<N>::getLowerPosition(const double& height) const {
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

template<size_t N>
Position PositionDeskControllerParams<N>::getHighestPosition() const {
  RevPositionIterator it = revPositions.end();
  if (it == revPositions.begin()) {
    // empty
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  it--;
  return std::make_pair(it->second, it->first);
}

template<size_t N>
Position PositionDeskControllerParams<N>::getLowestPosition() const {
  RevPositionIterator it = revPositions.begin();
  if (it == revPositions.end()) {
    // empty
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  return std::make_pair(it->second, it->first);
}

template<size_t N>
void PositionDeskControllerParams<N>::insertPosition(const String& name, const double& height) {
  // TODO validate min/max
  positions[name]      = height;
  revPositions[height] = name;
}

template<size_t N>
void PositionDeskControllerParams<N>::erasePosition(const String& name) {
  const double& height = positions[name];
  revPositions.erase(height);
  positions.erase(name);
}

template<size_t N>
void PositionDeskControllerParams<N>::setRevPositions() {
  revPositions.clear();
  
  for (PositionIterator it = positions.begin(); it != positions.end(); it++) {
    revPositions[it->second] = it->first;
  }
}


template<size_t N>
PositionDeskController<N>::PositionDeskController(const Params& params, const double& initialHeight)
    : HeightDeskController(params, initialHeight),
      params(params) {}

template<size_t N>
PositionDeskController<N>::PositionDeskController(const Params& params, const double& initialHeight, const double& initialTargetHeight)
    : HeightDeskController(params, initialHeight, initialTargetHeight),
      params(params) {}

template<size_t N>
String PositionDeskController<N>::getTargetPosition() const {
  return params.getPosition(getTargetHeight()).first;
}

template<size_t N>
String PositionDeskController<N>::getCurrentPosition() const {
  return params.getPosition(getCurrentHeight()).first;
}

template<size_t N>
boolean PositionDeskController<N>::isAtTargetPosition() const {
  return isAtTargetHeight() && getTargetPosition() != String();
}

template<size_t N>
void PositionDeskController<N>::setPosition(const String& newPositionName) {
  const Position newPosition = params.getPosition(newPositionName);
  setHeightImpl(newPosition);
}

template<size_t N>
void PositionDeskController<N>::raisePosition() {
  const Position newPosition = params.getHigherPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setHeightImpl(newPosition);
}

template<size_t N>
void PositionDeskController<N>::lowerPosition() {
  const Position newPosition = params.getLowerPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setHeightImpl(newPosition);
}

template<size_t N>
void PositionDeskController<N>::setHighestPosition() {
  setHeightImpl(params.getHighestPosition());
}

template<size_t N>
void PositionDeskController<N>::setLowestPosition() {
  setHeightImpl(params.getLowestPosition());
}

template<size_t N>
void PositionDeskController<N>::setHeightImpl(const Position& newPosition) {
  if (!isnan(newPosition.second)) {
    HeightDeskController::setHeight(newPosition.second);
  }
}

