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
  
  BaseDeskControllerParams();
  
  BaseDeskControllerParams(const char& upPin, const char& downPin);
};


class BaseDeskController {
public:
  typedef BaseDeskControllerParams Params;
  
  const Params params;
  
  BaseDeskController(const Params& params);
  
  virtual ~BaseDeskController();
  
  boolean isEnabled() const;
  
  DeskDrivingDirection getDrivingDirection() const;
  
  boolean isDriving() const;
  boolean isDrivingUp() const;
  boolean isDrivingDown() const;
  
  virtual void setEnabled(const boolean& newEnabled);
  void toggleEnabled();
  
  virtual void setDrivingDirection(const DeskDrivingDirection& direction);
  
  void startDriveUp();
  void startDriveDown();
  void stopDrive();
  
private:
  boolean enabled;
  DeskDrivingDirection drivingDirection;
};

#endif

