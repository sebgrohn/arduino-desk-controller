
const double minHeight   = 0.685;  // in meters
const double maxHeight   = 1.175;  // in meters
const double sitHeight   = 0.700;  // in meters
const double standHeight = 1.110;  // in meters

const char enableInputPin = 4;
const char upInputPin     = 3;
const char downInputPin   = 2;

const char upControlPin   = 7;
const char downControlPin = 6;
const char statusLedPin   = 5;
const char syncLedPin     = LED_BUILTIN;

const String TIME_HEADER  = "T";  // header tag for serial time sync message
const char   TIME_REQUEST = 7;    // ASCII bell character requests a time sync message

const char CURRENT_HEIGHT_EEPROM_ADDRESS = 0;

Bounce enableDebouncer = Bounce();
Bounce upDebouncer     = Bounce();
Bounce downDebouncer   = Bounce();

double initialHeight;
const int ignored = EEPROM_readAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, initialHeight);

const HeightDeskControllerParams controllerParams(
  upControlPin, downControlPin,
  minHeight, maxHeight, 0.0295, 0.0335);
//TimedDeskController controller2(controllerParams);
HeightDeskController controller(controllerParams, !isnan(initialHeight) ? initialHeight : controllerParams.minHeight);

void setupAlarm(const int& hours, const int& minutes, void (*function)()) {
  const boolean success = (Alarm.alarmRepeat(hours, minutes, 0, function) != dtINVALID_ALARM_ID);
  
  Serial.print("\t");
  printDigits(Serial, hours);
  Serial.print(":");
  printDigits(Serial, minutes);
  Serial.print(" ");
  Serial.print(function != setStandDeskHeight ? (function != setSitDeskHeight ? "?" : "v") : "^");
  Serial.print(success ? "" : " FAILED");
}


void setup()  {
  Serial.begin(57600);
  
  printDateTime(Serial);
  Serial.print(" Initial height: ");
  printLength(Serial, initialHeight);
  Serial.println();
  
  setSyncProvider(requestSync);  //set function to call when sync required
  printDateTime(Serial);
  Serial.println(" Waiting for time sync...");
  
  setupDebouncer(enableDebouncer, enableInputPin);
  setupDebouncer(upDebouncer, upInputPin);
  setupDebouncer(downDebouncer, downInputPin);
  
  pinMode(statusLedPin, OUTPUT);
  pinMode(syncLedPin, OUTPUT);
  
  controller.setEnabled(enableDebouncer.read() == LOW);
}

void loop()  {
  if (Serial.available()) {
    processSyncMessage();
  }
  
  const boolean enableChanged = enableDebouncer.update();
  const boolean upChanged     = upDebouncer.update();
  const boolean downChanged   = downDebouncer.update();
  
  const boolean reachedTargetHeight = controller.update();
  
  const boolean enable = (enableDebouncer.read() == LOW);
  const boolean up     = (upDebouncer.read() == LOW);
  const boolean down   = (downDebouncer.read() == LOW);
  
  if (enableChanged) {
    printDateTime(Serial);
    Serial.println(enable ? " Enabled" : " Disabled");
    
    controller.setEnabled(enable);
    
    if (enable && !controller.isAtTargetHeight()) {
      const double targetHeight = controller.getTargetHeight();
      
      printDateTime(Serial);
      Serial.print(" Saving height: ");
      printLength(Serial, targetHeight);
      Serial.println();
      
      EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, targetHeight);
    }
  }
  
  if (upChanged && up) {
    switch (controller.getDrivingDirection()) {
    case UP:
      break;
      
    case DOWN:
      stopDesk();
      break;
      
    case NONE:
      const double height = (controller.isAtTargetHeight() ? controller.getTargetHeight() : controller.getCurrentHeight());
      
      if (height < sitHeight) {
        setDeskHeight(sitHeight);
      } else if (height < standHeight) {
        setDeskHeight(standHeight);
      } else if (height < controllerParams.maxHeight) {
        setDeskHeight(controllerParams.maxHeight);
      }
      break;
    }
  }
  
  if (downChanged && down) {
    switch (controller.getDrivingDirection()) {
    case UP:
      stopDesk();
      break;
      
    case NONE:
      const double height = (controller.isAtTargetHeight() ? controller.getTargetHeight() : controller.getCurrentHeight());
      
      if (height > standHeight) {
        setDeskHeight(standHeight);
      } else if (height > sitHeight) {
        setDeskHeight(sitHeight);
      } else if (height > controllerParams.minHeight) {
        setDeskHeight(controllerParams.minHeight);
      }
      break;
    }
  }
  
  digitalWrite(syncLedPin, timeStatus() == timeSet ? HIGH : LOW);
  digitalWrite(statusLedPin, controller.isDriving() || upDebouncer.read() == LOW || downDebouncer.read() == LOW ? HIGH : LOW);
  
  // Alarm.delay() instead of built-in delay(), so that alarms are processed timely.
  Alarm.delay(30);
}


time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

void processSyncMessage() {
  if (Serial.read() == TIME_HEADER[0]) {
    const boolean firstTime = (timeStatus() == timeNotSet);
    
    setTime(Serial.parseInt());
    
    if (firstTime) {
      printDateTime(Serial);
      Serial.println(" Time synced, setting alarms:");
      
      // 25 stå, 25-35 sitta
      setupAlarm( 8, 25, setSitDeskHeight);    // 25
      setupAlarm(10, 25, setSitDeskHeight);    // 25
      setupAlarm(13, 10, setSitDeskHeight);    // 35
      setupAlarm(15, 25, setSitDeskHeight);    // 35
      Serial.println();
      
      setupAlarm( 8, 50, setStandDeskHeight);  // 25
      setupAlarm(10, 50, setStandDeskHeight);  // 25
      setupAlarm(13, 45, setStandDeskHeight);  // 25
      setupAlarm(16,  0, setStandDeskHeight);  // 25
      Serial.println();
      
      setupAlarm( 9, 15, setSitDeskHeight);    // 30
      setupAlarm(11, 15, setSitDeskHeight);    // 30 + 1h lunch
      setupAlarm(14, 10, setSitDeskHeight);    // 35
      setupAlarm(16, 25, setSitDeskHeight);    // 35
      Serial.println();
      
      setupAlarm( 9, 45, setStandDeskHeight);  // 25 + 15 fika
      setupAlarm(12, 45, setStandDeskHeight);  // 25
      setupAlarm(14, 45, setStandDeskHeight);  // 25 + 15 fika
      setupAlarm(17,  0, setStandDeskHeight);  // 
      Serial.println();
    } else {
      printDateTime(Serial);
      Serial.println(" Time synced");
    }
  }
}

void printDateTime(Print& print, const time_t& time) {
  if (time != dtINVALID_TIME) {
    print.print(year(time));
    print.print("-");
    printDigits(print, month(time));
    print.print("-");
    printDigits(print, day(time));
    print.print(" ");
    printDigits(print, hour(time));
    print.print(":");
    printDigits(print, minute(time));
    print.print(":");
    printDigits(print, second(time));
  } else {
    print.print("                   ");
  }
}
void printDateTime(Print& print) {
  if (timeStatus() != timeNotSet) {
    printDateTime(print, now());
  } else {
    print.print("xxxx-xx-xx --:--:--");
  }
}
void printDigits(Print& print, int digits) {
  if(digits < 10) {
    print.print('0');
  }
  print.print(digits);
}
void printLength(Print& print, const double& length) {
  if (length == sitHeight) {
    print.print("Sit   ");
  } else if (length == standHeight) {
    print.print("Stand ");
  } else if (length == controllerParams.minHeight) {
    print.print("Min   ");
  } else if (length == controllerParams.maxHeight) {
    print.print("Max   ");
  } else {
    print.print(length, 2);
    print.print(" m");
  }
}

void setupDebouncer(Bounce& debouncer, const int& pin) {
  pinMode(pin, INPUT_PULLUP);
  debouncer.attach(pin);
  debouncer.interval(5);
}

void stopDesk() {
  printDateTime(Serial);
  Serial.print(" Stopped at: ");
  printLength(Serial, controller.getCurrentHeight());
  Serial.println();
  
  controller.stopDrive();
  
  if (controller.isEnabled() && !controller.isAtTargetHeight()) {
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, controller.getCurrentHeight());
  }
}

void setDeskHeight(const double& targetHeight) {
  printDateTime(Serial);
  Serial.print(" Driving to: ");
  printLength(Serial, targetHeight);
  Serial.println();
  
  controller.setHeight(targetHeight);
  
  if (controller.isEnabled() && !controller.isAtTargetHeight()) {
    printDateTime(Serial);
    Serial.print(" Saving height: ");
    printLength(Serial, targetHeight);
    Serial.println();
    
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, targetHeight);
  }
}
void setSitDeskHeight() { setDeskHeight(sitHeight); }
void setStandDeskHeight() { setDeskHeight(standHeight); }


