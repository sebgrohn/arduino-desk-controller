#ifndef HeightDeskController_h
#define HeightDeskController_h

#include <Arduino.h>

#include "BaseDeskController.h"


struct DeskControllerParams {

  /// The lowest possible height if the desk, in m.
  const double minHeight;
  
  /// The highest possible height if the desk, in m.
  const double maxHeight;
  
  // The speed with which the desk is raised, in m/s.
  const double upSpeed;
  
  // The speed with which the desk is lowered, in m/s.
  const double downSpeed;
  
  DeskControllerParams(const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed);
  
  double getHeightDiff() const;
  
  double getTimeForHeightDiff(const double& heightDiff) const;
  
  double getTimeForFullHeightDiff(const DeskDrivingDirection& direction) const;
  
  double getHeightDiffForTime(const double& timeDiff, const DeskDrivingDirection& direction) const;
};


class HeightDeskController : public BaseDeskController {
public:
  const DeskControllerParams params;
  
  HeightDeskController(const DeskControllerParams& params, const int& upPin, const int& downPin, const double& initialHeight);
  
  double getTargetHeight() const;
  
  double getCurrentHeight() const;
  
  boolean isAtTargetHeight() const;
  
  void setHeight(const double& newHeight);
  
  void resumeDrive();
  
  /*virtual*/ void update();
  
  /*virtual*/ void setEnabled(const boolean& newEnabled);
  
  /*virtual*/ void startDrive(const DeskDrivingDirection& direction);
  
  /*virtual*/ void stopDrive();
  
private:
  static const double HEIGHT_DIFF_TOLERANCE = 0.01;
  
  double targetHeight;
  double lastStoppedHeight;
  unsigned long startDrivingTime;
};

#endif

