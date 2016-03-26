
#include "PositionDeskController.h"

#include <limits>
#include <cmath>


const Position Position::empty = Position(String(), std::numeric_limits<double>::quiet_NaN());

Position::Position(const String& name, const double& height)
    : name(name),
      height(height) {}


template<size_t N>
Position PositionDeskControllerParams<N>::getPosition(const String& name) const {
  for (int i = 0; i < numPositions; i++) {
    if (positions[i].name == name) {
      return positions[i];
    }
  }
  // empty / fail-name
  return Position::empty;
}

template<size_t N>
Position PositionDeskControllerParams<N>::getPosition(const double& height) const {
  for (int i = 0; i < numPositions; i++) {
    if (positions[i].height == height) {
      return positions[i];
    }
  }
  // empty / fail-height
  return Position::empty;
}

template<size_t N>
Position PositionDeskControllerParams<N>::getHigherPosition(const double& height) const {
  for (int i = 0; i < numPositions; i++) {
    if (positions[i].height == height && i == numPositions - 1) {
      // already highest, return same
      return positions[i];
    }
    if (positions[i].height == height && i < numPositions - 1) {
      return positions[i + 1];
    }
    if (positions[i].height > height) {
      return positions[i];
    }
  }
  // empty / higher than highest
  return Position::empty;
}

template<size_t N>
Position PositionDeskControllerParams<N>::getLowerPosition(const double& height) const {
  for (int i = numPositions - 1; i >= 0; i--) {
    if (positions[i].height == height && i == 0) {
      // already lowest, return same
      return positions[i];
    }
    if (positions[i].height == height && i > 0) {
      return positions[i - 1];
    }
    if (positions[i].height < height) {
      return positions[i];
    }
  }
  // empty / lower than lowest
  return Position::empty;
}

template<size_t N>
int PositionDeskControllerParams<N>::insertPosition(const String& name, const double& height) {
  if (numPositions == maxPositions) {
    // positions array full
    return maxPositions;
  }
  if (numPositions > 0 && positions[numPositions - 1].height >= height) {
    // not inserted in order (lowest to highest)
    return -1;
  }
  
  const size_t insertIndex = numPositions;
  numPositions++;
  positions[insertIndex] = Position(name, constrain(height, minHeight, maxHeight));
  return insertIndex;
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
  return params.getPosition(getTargetHeight()).name;
}

template<size_t N>
String PositionDeskController<N>::getCurrentPosition() const {
  return params.getPosition(getCurrentHeight()).name;
}

template<size_t N>
boolean PositionDeskController<N>::isAtTargetPosition() const {
  return isAtTargetHeight() && getTargetPosition() != String();
}

template<size_t N>
void PositionDeskController<N>::setTargetPosition(const String& newTargetPositionName) {
  const auto newTargetPosition = params.getPosition(newTargetPositionName);
  setTargetHeightImpl(newTargetPosition);
}

template<size_t N>
void PositionDeskController<N>::raisePosition() {
  const auto newPosition = params.getHigherPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setTargetHeightImpl(newPosition);
}

template<size_t N>
void PositionDeskController<N>::lowerPosition() {
  const auto newPosition = params.getLowerPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setTargetHeightImpl(newPosition);
}

template<size_t N>
void PositionDeskController<N>::setTargetHeightImpl(const Position& newTargetPosition) {
  if (!isnan(newTargetPosition.height)) {
    HeightDeskController::setTargetHeight(newTargetPosition.height);
  }
}

