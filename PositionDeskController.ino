
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

PositionDeskControllerParams::position PositionDeskControllerParams::getHigherPosition(const String& name) const {
  rev_position_map::const_iterator it = revPositions.find(getPosition(name).second);
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

PositionDeskControllerParams::position PositionDeskControllerParams::getLowerPosition(const String& name) const {
  rev_position_map::const_iterator it = revPositions.find(getPosition(name).second);
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

// std::map<>::lower_bound(...) vs upper_bound(...) (L vs U)
// 
//    begin
//    "min"     "sit"    "stand"    "max"      end
//      |---------|---------|---------|         |
//                                         ^   LU
//                          l        L^         U
//                          l    ^   LU
//                l        L^         U
//                l    ^   LU
//      l        L^         U
//      l    ^   LU
//     L^         U
// ^   LU
//      |---------|---------|---------|         |
//    "min"     "sit"    "stand"    "max"      end
//    begin

PositionDeskControllerParams::position PositionDeskControllerParams::getHigherPosition(const double& height) const {
  rev_position_map::const_iterator itLower = revPositions.lower_bound(height);
  rev_position_map::const_iterator itUpper = revPositions.upper_bound(height);
  
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

PositionDeskControllerParams::position PositionDeskControllerParams::getLowerPosition(const double& height) const {
  rev_position_map::const_iterator itLower = revPositions.lower_bound(height);
  rev_position_map::const_iterator itUpper = revPositions.upper_bound(height);
  
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

PositionDeskControllerParams::position PositionDeskControllerParams::getHighestPosition() const {
  rev_position_map::const_iterator it = revPositions.end();
  if (it == revPositions.begin()) {
    // empty
    return std::make_pair(String(), std::numeric_limits<double>::quiet_NaN());
  }
  it--;
  return std::make_pair(it->second, it->first);
}

PositionDeskControllerParams::position PositionDeskControllerParams::getLowestPosition() const {
  rev_position_map::const_iterator it = revPositions.begin();
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
  
  for (position_map::const_iterator it = positions.begin(); it != positions.end(); it++) {
    revPositions[it->second] = it->first;
  }
}

void PositionDeskControllerParams::debugPrint(Print& printer) const {
  printer.println(F("\tpositions:"));
  for(position_map::const_iterator it = positions.begin(); it != positions.end(); it++) {
    printPosition(printer, *it);
  }
  printer.println();
  
  printer.println(F("\trevPositions:"));
  for(rev_position_map::const_iterator it = revPositions.begin(); it != revPositions.end(); it++) {
    printRevPosition(printer, *it);
  }
  printer.println();
  
  position pos;
  
  printer.println(F("\ttraverse up (name):"));
  pos = getLowestPosition();
  printPosition(printer, pos);
  pos = getHigherPosition(pos.first);
  printPosition(printer, pos);
  pos = getHigherPosition(pos.first);
  printPosition(printer, pos);
  pos = getHigherPosition(pos.first);
  printPosition(printer, pos);
  pos = getHigherPosition(pos.first);
  printPosition(printer, pos);
  printer.println();
  
  printer.println(F("\ttraverse down (name):"));
  pos = getHighestPosition();
  printPosition(printer, pos);
  pos = getLowerPosition(pos.first);
  printPosition(printer, pos);
  pos = getLowerPosition(pos.first);
  printPosition(printer, pos);
  pos = getLowerPosition(pos.first);
  printPosition(printer, pos);
  pos = getLowerPosition(pos.first);
  printPosition(printer, pos);
  printer.println();
  
  printer.println(F("\ttraverse up (height):"));
  pos = getLowestPosition();
  printPosition(printer, pos);
  pos = getHigherPosition(pos.second);
  printPosition(printer, pos);
  pos = getHigherPosition(pos.second);
  printPosition(printer, pos);
  pos = getHigherPosition(pos.second);
  printPosition(printer, pos);
  pos = getHigherPosition(pos.second);
  printPosition(printer, pos);
  printer.println();
  
  printer.println(F("\ttraverse down (height):"));
  pos = getHighestPosition();
  printPosition(printer, pos);
  pos = getLowerPosition(pos.second);
  printPosition(printer, pos);
  pos = getLowerPosition(pos.second);
  printPosition(printer, pos);
  pos = getLowerPosition(pos.second);
  printPosition(printer, pos);
  pos = getLowerPosition(pos.second);
  printPosition(printer, pos);
}
void PositionDeskControllerParams::printPosition(Print& printer, const position& position) const {
  printer.print(F("\t\""));
  printer.print(position.first);
  printer.print(F("\"\t=> "));
  printer.print(position.second);
  printer.print(F("\t<=> "));
  printer.print(revPositions.find(position.second)->first);
  printer.print(F("\t=> \""));
  printer.print(revPositions.find(position.second)->second);
  printer.print(F("\"\t| "));
  printer.print(position.first == revPositions.find(position.second)->second);
  printer.print(' ');
  printer.print(position.second == revPositions.find(position.second)->first);
  printer.println();
}
void PositionDeskControllerParams::printRevPosition(Print& printer, const std::pair<double, String>& revPosition) const {
  printer.print(F("\t"));
  printer.print(revPosition.first);
  printer.print(F("\t=> \""));
  printer.print(revPosition.second);
  printer.print(F("\"\t<=> \""));
  printer.print(positions.find(revPosition.second)->first);
  printer.print(F("\"\t=> "));
  printer.print(positions.find(revPosition.second)->second);
  printer.print(F("\t| "));
  printer.print(revPosition.first == positions.find(revPosition.second)->second);
  printer.print(' ');
  printer.print(revPosition.second == positions.find(revPosition.second)->first);
  printer.println();
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
  const PositionDeskControllerParams::position newPosition = params.getPosition(newPositionName);
  setHeightImpl(newPosition);
}

void PositionDeskController::raisePosition() {
  const PositionDeskControllerParams::position newPosition = params.getHigherPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setHeightImpl(newPosition);
}

void PositionDeskController::lowerPosition() {
  const PositionDeskControllerParams::position newPosition = params.getLowerPosition(isAtTargetHeight() ? getTargetHeight() : getCurrentHeight());
  setHeightImpl(newPosition);
}

void PositionDeskController::setHighestPosition() {
  setHeightImpl(params.getHighestPosition());
}

void PositionDeskController::setLowestPosition() {
  setHeightImpl(params.getLowestPosition());
}

void PositionDeskController::setHeightImpl(const PositionDeskControllerParams::position& newPosition) {
  if (!isnan(newPosition.second)) {
    HeightDeskController::setHeight(newPosition.second);
  }
}

