#ifndef BaseDeskController_h
#define BaseDeskController_h

#include <Arduino.h>


enum DeskDrivingDirection {
  DOWN = -1,
  NONE = 0,
  UP   = 1
};


class BaseDeskController {
public:
  const int upPin;
  const int downPin;
  
  BaseDeskController(const int& upPin, const int& downPin);
  
  boolean isEnabled() const;
  
  DeskDrivingDirection getDrivingDirection() const;
  
  boolean isDriving() const;
  
  boolean isDrivingUp() const;
  
  boolean isDrivingDown() const;
  
  /*virtual*/ void setEnabled(const boolean& newEnabled);
  
  /*virtual*/ void startDrive(const DeskDrivingDirection& direction);
  
  /*virtual*/ void stopDrive();
  
private:
  boolean enabled;
  DeskDrivingDirection drivingDirection;
};

#endif

