#ifndef PositionDeskController_h
#define PositionDeskController_h

#include <Arduino.h>
#include <StandardCplusplus.h>
#include <map>

#include "HeightDeskController.h"


typedef std::map<String, double> PositionMap;
typedef std::pair<String, double> Position;


class PositionDeskControllerParams : public HeightDeskControllerParams {
public:
  PositionDeskControllerParams();
  
  PositionDeskControllerParams(const char& upPin, const char& downPin);
  
  PositionDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed);
  
  PositionDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
    const PositionMap& positions);
  
  Position getPosition(const String& name) const;
  Position getPosition(const double& height) const;
  
  Position getHigherPosition(const String& name) const;
  Position getLowerPosition(const String& name) const;
  
  Position getHigherPosition(const double& height) const;
  Position getLowerPosition(const double& height) const;
  
  Position getHighestPosition() const;
  Position getLowestPosition() const;
  
  void insertPosition(const String& name, const double& height);
  void erasePosition(const String& name);
  
private:
  typedef std::map<double, String> RevPositionMap;
  typedef RevPositionMap::const_iterator RevPositionIterator;
  typedef PositionMap::const_iterator PositionIterator;
  
  PositionMap    positions;
  RevPositionMap revPositions;
  
  void setRevPositions();
};


class PositionDeskController : public HeightDeskController {
public:
  const PositionDeskControllerParams params;
  
  PositionDeskController(const PositionDeskControllerParams& params, const double& initialHeight);
  PositionDeskController(const PositionDeskControllerParams& params, const double& initialHeight, const double& initialTargetHeight);
  
  String getTargetPosition() const;
  String getCurrentPosition() const;
  boolean isAtTargetPosition() const;
  
  void setPosition(const String& newPositionName);
  
  void raisePosition();
  void lowerPosition();
  
  void setHighestPosition();
  void setLowestPosition();
  
private:
  void setHeightImpl(const Position& newPosition);
};

#endif

