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
  const BaseDeskControllerParams params;
  
  BaseDeskController(const BaseDeskControllerParams& params);
  
  virtual ~BaseDeskController();
  
  boolean isEnabled() const;
  
  DeskDrivingDirection getDrivingDirection() const;
  
  boolean isDriving() const;
  
  boolean isDrivingUp() const;
  
  boolean isDrivingDown() const;
  
  virtual void setEnabled(const boolean& newEnabled);
  
  virtual void startDrive(const DeskDrivingDirection& direction);
  
  virtual void stopDrive();
  
private:
  boolean enabled;
  DeskDrivingDirection drivingDirection;
};

#endif

