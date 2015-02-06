#ifndef TimedDeskController_h
#define TimedDeskController_h

#include <Arduino.h>

#include "BaseDeskController.h"


class TimedDeskController : public BaseDeskController {
public:
  TimedDeskController(const int& upPin, const int& downPin);
  
  double getCurrentTimeDiff() const;
  
  void startDriveTime(const double& newTimeDiff, const DeskDrivingDirection& direction);
  
  virtual boolean update();
  
  virtual void startDrive(const DeskDrivingDirection& direction);
  
  virtual void stopDrive();
  
private:
  double targetTimeDiff;
  unsigned long startDrivingTime;
  unsigned long stopDrivingTime;
  
  boolean shouldStop() const;
};

#endif

