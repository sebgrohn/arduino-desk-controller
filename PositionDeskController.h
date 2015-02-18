#ifndef PositionDeskController_h
#define PositionDeskController_h

#include <Arduino.h>
#include <StandardCplusplus.h>
#include <map>
#include <utility>

#include "HeightDeskController.h"


class PositionDeskControllerParams : public HeightDeskControllerParams {
public:
  typedef std::map<String, double> position_map;
  typedef std::pair<String, double> position;
  
  PositionDeskControllerParams();
  
  PositionDeskControllerParams(const char& upPin, const char& downPin);
  
  PositionDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed);
  
  PositionDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
    const position_map& positions);
  
  position getPosition(const String& name) const;
  position getPosition(const double& height) const;
  
  position getHigherPosition(const String& name) const;
  position getLowerPosition(const String& name) const;
  
  position getHigherPosition(const double& height) const;
  position getLowerPosition(const double& height) const;
  
  position getHighestPosition() const;
  position getLowestPosition() const;
  
  void insertPosition(const String& name, const double& height);
  void erasePosition(const String& name);
  
private:
  typedef std::map<double, String> rev_position_map;
  
  position_map     positions;
  rev_position_map revPositions;
  
  void setRevPositions();
};


class PositionDeskController : public HeightDeskController {
public:
  const PositionDeskControllerParams params;
  
  PositionDeskController(const PositionDeskControllerParams& params, const double& initialHeight);
  
  String getTargetPosition() const;
  String getCurrentPosition() const;
  boolean isAtTargetPosition() const;
  
  void setPosition(const String& newPositionName);
  
  void raisePosition();
  void lowerPosition();
  
  void setHighestPosition();
  void setLowestPosition();
  
private:
  void setHeight(const PositionDeskControllerParams::position& newPosition);
};

#endif

