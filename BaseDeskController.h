#ifndef BaseDeskController_h
#define BaseDeskController_h

#include <Arduino.h>


enum DeskDrivingDirection {
  DOWN = -1,
  NONE =  0,
  UP   =  1
};


struct BaseDeskControllerParams {
  char upPin;
  char downPin;
  
  BaseDeskControllerParams() {}
  BaseDeskControllerParams(const char& upPin, const char& downPin);
};


class BaseDeskController {
public:
  typedef BaseDeskControllerParams Params;
  
  Params params;
  
  BaseDeskController(const Params& params);
  
  virtual ~BaseDeskController() {}
  
  boolean isEnabled() const { return enabled; }
  
  DeskDrivingDirection getDrivingDirection() const { return drivingDirection; }
  
  boolean isDriving() const     { return drivingDirection != NONE; }
  boolean isDrivingUp() const   { return drivingDirection == UP; }
  boolean isDrivingDown() const { return drivingDirection == DOWN; }
  
  virtual void setEnabled(const boolean& newEnabled);
  void toggleEnabled();
  
  virtual void setDrivingDirection(const DeskDrivingDirection& newDirection);
  
  void driveUp()   { setDrivingDirection(UP); }
  void driveDown() { setDrivingDirection(DOWN); }
  void stop()      { setDrivingDirection(NONE); }
  
private:
  boolean enabled;
  DeskDrivingDirection drivingDirection;
};

#endif

