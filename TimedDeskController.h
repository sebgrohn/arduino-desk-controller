#ifndef TimedDeskController_h
#define TimedDeskController_h

#include <Arduino.h>

#include "BaseDeskController.h"


class TimedDeskController : public BaseDeskController {
public:
  TimedDeskController(const BaseDeskControllerParams& params);
  
  double getCurrentTimeDiff() const;
  
  void setDriveTimeInterval(const double& newTimeInterval, const DeskDrivingDirection& direction);
  
  virtual boolean update();
  
  virtual void setDrivingDirection(const DeskDrivingDirection& direction);
  
private:
  double targetTimeInterval;
  unsigned long startDrivingTime;
  unsigned long stopDrivingTime;
  
  boolean shouldStop() const;
};

#endif

