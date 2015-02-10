#ifndef HeightDeskController_h
#define HeightDeskController_h

#include <Arduino.h>

#include "BaseDeskController.h"


struct HeightDeskControllerParams : public BaseDeskControllerParams {

  /// The lowest possible height of the desk, in m.
  double minHeight;
  
  /// The highest possible height of the desk, in m.
  double maxHeight;
  
  // The speed with which the desk is raised, in m/s.
  double upSpeed;
  
  // The speed with which the desk is lowered, in m/s.
  double downSpeed;
  
  HeightDeskControllerParams();
  
  HeightDeskControllerParams(const char& upPin, const char& downPin);
  
  HeightDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed);
  
  double getHeightDiff() const;
  
  double getTimeIntervalForHeightDiff(const double& heightDiff) const;
  
  double getTimeIntervalForFullHeightDiff(const DeskDrivingDirection& direction) const;
  
  double getHeightDiffForTimeInterval(const double& timeInterval, const DeskDrivingDirection& direction) const;
};


class HeightDeskController : public BaseDeskController {
public:
  const HeightDeskControllerParams params;
  
  HeightDeskController(const HeightDeskControllerParams& params, const double& initialHeight);
  
  double getTargetHeight() const;
  
  double getCurrentHeight() const;
  
  boolean isAtTargetHeight() const;
  
  void setHeight(const double& newHeight);
  
  void resumeDrive();
  
  virtual boolean update();
  
  virtual void setEnabled(const boolean& newEnabled);
  
  virtual void setDrivingDirection(const DeskDrivingDirection& direction);
  
private:
  static const double HEIGHT_DIFF_TOLERANCE = 0.001;
  
  double targetHeight;
  double lastStoppedHeight;
  unsigned long startDrivingTime;
};

#endif

