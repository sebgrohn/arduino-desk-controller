#ifndef TimerDeskController_h
#define TimerDeskController_h

#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>

#include <TimeAlarms.h>

#include "PositionDeskController.h"


template <class Operation>
class binderSingle
{
protected:
  Operation op;
  typename Operation::argument_type value;
  
public:
  binderSingle(const Operation& x, const typename Operation::argument_type& y)
    : op (x),
      value(y) {}
  
  void operator() () const { op(value); }
};

template <class Operation, class T>
binderSingle<Operation> bindSingle (const Operation& op, const T& x)
{
  return binderSingle<Operation>(op, typename Operation::argument_type(x));
}


struct TimerDeskControllerParams : public PositionDeskControllerParams {
  
  String sitPositionName;
  
  String standPositionName;
  
  /// The ideal time for sitting in front of the desk, in s.
  double sitTimeInterval;
  
  /// The ideal time for standing in front of the desk, in s.
  double standTimeInterval;
  
  TimerDeskControllerParams();
  
  TimerDeskControllerParams(const char& upPin, const char& downPin);
    
  TimerDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed);
  
  TimerDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
    const PositionMap& positions);
  
  TimerDeskControllerParams(
    const char& upPin, const char& downPin,
    const double& minHeight, const double& maxHeight, const double& upSpeed, const double& downSpeed,
    const PositionMap& positions,
    const String& sitPositionName, const String& standPositionName, const double& sitTimeInterval, const double& standTimeInterval);
};


class TimerDeskController : public PositionDeskController {
public:
  const TimerDeskControllerParams params;
  
  TimerDeskController(const TimerDeskControllerParams& params, const double& initialHeight);
  TimerDeskController(const TimerDeskControllerParams& params, const double& initialHeight, const double& initialTargetHeight);
  
  double getNextHeight() const;
  String getNextPosition() const;
  time_t getNextPositionChangeTime() const;
  
  // TODO private?
  void scheduleNextPositionChange();
  
  virtual boolean update();
  
  //virtual void setEnabled(const boolean& newEnabled);
  
  //virtual void setDrivingDirection(const DeskDrivingDirection& direction);
  
private:
  typedef std::vector<TimerDeskController *> controller_vector;
  
  static const unsigned int numControllerInstances = 2;
  static controller_vector controllerInstances;
  
  static void processPositionChangeStatic(const int& index);
  static OnTick_t getProcessPositionChangePtr(TimerDeskController * const controller);
  static void processPositionChange_0();
  static void processPositionChange_1();
  
  time_t changedPositionTime;
  String nextPosition;
  AlarmId alarmId;
  time_t alarmTime;
  
  void processPositionChange();
  
  // DEBUG remove
  void printDigits(Print& printer, const int& digits) {
    if(digits < 10) {
      printer.print('0');
    }
    printer.print(digits);
  }
  void printTimeShort(Print& printer, const time_t& time) {
    if (time != dtINVALID_TIME) {
      printDigits(printer, hour(time));
      printer.print(':');
      printDigits(printer, minute(time));
      printer.print(':');
      printDigits(printer, second(time));
    } else {
      printer.print(F("     "));
    }
  }
};

#endif

