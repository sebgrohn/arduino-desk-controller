#ifndef PositionDeskController_h
#define PositionDeskController_h

#include <Arduino.h>
#include <StandardCplusplus.h>
#include <map>

#include "HeightDeskController.h"


struct Position {
  static const Position empty;
  
  Position();
  Position(const String& name, const double& height);
  
  String name;
  double height;
};


template<size_t N>
class PositionDeskControllerParams : public HeightDeskControllerParams {
public:
  static const size_t maxPositions;
  
  PositionDeskControllerParams();
  
  PositionDeskControllerParams(const char& upPin, const char& downPin);
  
  PositionDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed);
  
  size_t getNumPositions() const;
  
  Position getPosition(const String& name) const;
  Position getPosition(const double& height) const;
  
  Position getHigherPosition(const double& height) const;
  Position getLowerPosition(const double& height) const;
  
  int insertPosition(const String& name, const double& height);
  
private:
  Position positions[N];
  size_t numPositions;
};


template<size_t N>
class PositionDeskController : public HeightDeskController {
public:
  typedef PositionDeskControllerParams<N> Params;
  
  Params params;
  
  PositionDeskController(const Params& params, const double& initialHeight);
  PositionDeskController(const Params& params, const double& initialHeight, const double& initialTargetHeight);
  
  String getTargetPosition() const;
  String getCurrentPosition() const;
  boolean isAtTargetPosition() const;
  
  void setPosition(const String& newPositionName);
  
  void raisePosition();
  void lowerPosition();
  
private:
  void setHeightImpl(const Position& newPosition);
};

#endif

