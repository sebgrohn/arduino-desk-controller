
#include "TimerDeskController.h"

#include <functional>


TimerDeskControllerParams::TimerDeskControllerParams() {}

TimerDeskControllerParams::TimerDeskControllerParams(const char& upPin, const char& downPin)
    : PositionDeskControllerParams(upPin, downPin) {}

TimerDeskControllerParams::TimerDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed)
    : PositionDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed) {}

TimerDeskControllerParams::TimerDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
  const PositionMap& positions)
    : PositionDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed, positions) {}

TimerDeskControllerParams::TimerDeskControllerParams(
  const char& upPin, const char& downPin,
  const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
  const PositionMap& positions,
  const String& sitPositionName, const String& standPositionName, const double& sitTimeInterval, const double& standTimeInterval)
    : PositionDeskControllerParams(upPin, downPin, minHeight, maxHeight, upSpeed, downSpeed, positions),
      sitPositionName(sitPositionName),
      standPositionName(standPositionName),
      sitTimeInterval(std::max(0.0, sitTimeInterval)),
      standTimeInterval(std::max(0.0, standTimeInterval)) {}


TimerDeskController::TimerDeskController(const TimerDeskControllerParams& params, const double& initialHeight)
    : TimerDeskController(params, initialHeight, initialHeight) {}

TimerDeskController::TimerDeskController(const TimerDeskControllerParams& params, const double& initialHeight, const double& initialTargetHeight)
    : PositionDeskController(params, initialHeight, initialTargetHeight),
      params(params) {
  if (controllerInstances.size() == numControllerInstances) {
    // TODO error
  }
  controllerInstances.push_back(this);
  
  changedPositionTime = dtINVALID_TIME;
  alarmId             = dtINVALID_ALARM_ID;
  
  //scheduleNextPositionChange();
}

double TimerDeskController::getNextHeight() const {
  return params.getPosition(nextPosition).second;
}

String TimerDeskController::getNextPosition() const {
  return nextPosition;
}

time_t TimerDeskController::getNextPositionChangeTime() const {
  return alarmTime;
}

boolean TimerDeskController::update() {
  const boolean reachedTargetHeight = HeightDeskController::update();
  if (reachedTargetHeight) {
    scheduleNextPositionChange();
  }
  return reachedTargetHeight;
}

//void TimerDeskController::setDrivingDirection(const DeskDrivingDirection& newDrivingDirection) {
//  if (newDrivingDirection != NONE && !isEnabled()) {
//    return;
//  }
//  BaseDeskController::setDrivingDirection(newDrivingDirection);
//  scheduleNextPositionChange();
//}

TimerDeskController::controller_vector TimerDeskController::controllerInstances = TimerDeskController::controller_vector();

void TimerDeskController::processPositionChangeStatic(const int& index) {
  // DEBUG remove
  Serial.print(F("\tProcessing: "));
  Serial.println(index);
  
  if (index < controllerInstances.size()) {
    controllerInstances[index]->processPositionChange();
  }
}
OnTick_t TimerDeskController::getProcessPositionChangePtr(TimerDeskController * const controller) {
  for (int i = 0; i < controllerInstances.size(); i++) {
    if (controller == controllerInstances[i]) {
      switch (i) {
      case 0:  return processPositionChange_0;
      case 1:  return processPositionChange_1;
      default: return NULL;
      }
    }
  }
  return NULL;
}
void TimerDeskController::processPositionChange_0() { processPositionChangeStatic(0); }
void TimerDeskController::processPositionChange_1() { processPositionChangeStatic(1); }

void TimerDeskController::scheduleNextPositionChange() {
  const String targetPosition = (isAtTargetPosition() ? getTargetPosition() : getCurrentPosition());
  double timeInterval;
  if (targetPosition == params.sitPositionName) {
    changedPositionTime = now();
    nextPosition        = params.standPositionName;
    timeInterval        = params.sitTimeInterval;
  } else if (targetPosition == params.standPositionName) {
    changedPositionTime = now();
    nextPosition        = params.sitPositionName;
    timeInterval        = params.standTimeInterval;
  } else {
    changedPositionTime = now();
    nextPosition        = params.standPositionName;
    timeInterval        = params.sitTimeInterval;
  }
  
  alarmTime = (changedPositionTime != dtINVALID_TIME ? changedPositionTime : now()) + timeInterval;
  
  // TODO adjust for lunch and pause times?
  
  if(alarmId == dtINVALID_ALARM_ID) {
    // DEBUG remove
    Serial.print(F("\tNext alarm time (first): "));
    printTimeShort(Serial, alarmTime);
    Serial.print(F(" \""));
    Serial.print(targetPosition);
    Serial.print(F("\" \""));
    Serial.print(nextPosition);
    Serial.println('"');
    
    alarmId = Alarm.alarmOnce(alarmTime, getProcessPositionChangePtr(this));
  } else {
    // DEBUG remove
    Serial.print(F("Next alarm time (other): "));
    printTimeShort(Serial, alarmTime);
    Serial.print(F(" \""));
    Serial.print(targetPosition);
    Serial.print(F("\" \""));
    Serial.print(nextPosition);
    Serial.println('"');
    
    Alarm.write(alarmId, alarmTime);
  }
}

void TimerDeskController::processPositionChange() {
  // DEBUG remove
  Serial.print(F("\tProcessing: "));
  Serial.println(nextPosition);
  
  alarmId = dtINVALID_ALARM_ID;
  setPosition(nextPosition);
}

