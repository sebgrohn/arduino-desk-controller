
const DeskControllerParams controllerParams(0.685, 1.175, 0.0304, 0.0325);

const double sitHeight   = 0.70;  // in meters
const double standHeight = 1.11;  // in meters

const int enableInputPin = 4;
const int upInputPin     = 3;
const int downInputPin   = 2;
const int upControlPin   = 7;
const int downControlPin = 6;
const int statusLedPin   = 5;
const int syncLedPin     = LED_BUILTIN;

const String TIME_HEADER = "T";  // header tag for serial time sync message
const int TIME_REQUEST = 7;      // ASCII bell character requests a time sync message

const int CURRENT_HEIGHT_EEPROM_ADDRESS = 0;

Bounce enableDebouncer = Bounce();
Bounce upDebouncer     = Bounce();
Bounce downDebouncer   = Bounce();

double initialHeight;
const int ign = EEPROM_readAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, initialHeight);

//TimedDeskController controller2(upControlPin, downControlPin);
HeightDeskController controller(controllerParams, upControlPin, downControlPin, initialHeight);

void setupAlarm(const int& hours, const int& minutes, void (*function)()) {
  boolean success = true;
  
  success &= (Alarm.alarmRepeat(hours, minutes, 0, function) != dtINVALID_ALARM_ID);
  
  Serial.print("                    ");
  Serial.print(function != setStandDeskHeight ? (function != setSitDeskHeight ? "Other" : "Lower") : "Raise");
  Serial.print(" at ");
  printDigits(hours);
  Serial.print(":");
  printDigits(minutes);
  Serial.print(" every day");
  
  if (success) {
    Serial.println(" SET");
  } else {
    Serial.println(" FAILED");
  }
}


void setup()  {
  Serial.begin(57600);
  
  Serial.print("Initial height set to ");
  Serial.print(initialHeight, 3);
  Serial.println(" m");
  
  setSyncProvider(requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message...");
  
  setupDebouncer(enableDebouncer, enableInputPin);
  setupDebouncer(upDebouncer, upInputPin);
  setupDebouncer(downDebouncer, downInputPin);
  
  pinMode(statusLedPin, OUTPUT);
  pinMode(syncLedPin, OUTPUT);
} 

void loop()  {
  if (Serial.available()) {
    processSyncMessage();
  }
  
  const boolean enableSwitchChanged = enableDebouncer.update();
  const boolean upSwitchChanged     = upDebouncer.update();
  const boolean downSwitchChanged   = downDebouncer.update();
  
  controller.update();
  
  controller.setEnabled(enableDebouncer.read() == LOW);
  
  /*if (enableDebouncer.read() == LOW && enableSwitchChanged) {
    double targetHeight = controller.getTargetHeight();
    
    Serial.print("Enabling and driving to target height ");
    Serial.print(targetHeight, 3);
    Serial.println(" m...");
    
    //double savedHeight;
    //const int ign = EEPROM_readAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, savedHeight);
  
    if (!controller.isAtTargetHeight()) {
      EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, targetHeight);
    }
  }*/
  
  const double targetHeight = controller.getTargetHeight();
  if (targetHeight == controllerParams.minHeight) {
    if (upDebouncer.read() == LOW && upSwitchChanged && !controller.isDrivingUp()) {
      setSitDeskHeight();
    }
  } else if (targetHeight == sitHeight) {
    if (upDebouncer.read() == LOW && upSwitchChanged && !controller.isDrivingUp()) {
      setStandDeskHeight();
    }
    if (downDebouncer.read() == LOW && downSwitchChanged && !controller.isDrivingDown()) {
      setMinDeskHeight();
    }
  } else if (targetHeight == standHeight) {
    if (upDebouncer.read() == LOW && upSwitchChanged && !controller.isDrivingUp()) {
      setMaxDeskHeight();
    }
    if (downDebouncer.read() == LOW && downSwitchChanged && !controller.isDrivingDown()) {
      setSitDeskHeight();
    }
  } else if (targetHeight == controllerParams.maxHeight) {
    if (downDebouncer.read() == LOW && downSwitchChanged && !controller.isDrivingDown()) {
      setStandDeskHeight();
    }
  }
    
  digitalWrite(syncLedPin, timeStatus() == timeSet ? HIGH : LOW);
  digitalWrite(statusLedPin, controller.isDriving() || upDebouncer.read() == LOW || downDebouncer.read() == LOW ? HIGH : LOW);
  
  Alarm.delay(30);
}


time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

void processSyncMessage() {
  if (Serial.read() == TIME_HEADER[0]) {
    const boolean firstTimeInit = (timeStatus() == timeNotSet);
    
    setTime(Serial.parseInt()); // Sync Arduino clock to the time received on the serial port
    
    if (firstTimeInit) {
      digitalClockDisplay();
      Serial.println("Time set, setting alarms:");
      
      setupAlarm(8, 15, setStandDeskHeight);
      setupAlarm(8, 40, setSitDeskHeight);
      setupAlarm(9, 20, setStandDeskHeight);
      setupAlarm(9, 45, setSitDeskHeight);
      
      setupAlarm(10, 30, setStandDeskHeight);
      setupAlarm(10, 55, setSitDeskHeight);
      setupAlarm(11, 20, setStandDeskHeight);
      setupAlarm(11, 45, setSitDeskHeight);
      
      setupAlarm(13, 20, setStandDeskHeight);
      setupAlarm(13, 45, setSitDeskHeight);
      setupAlarm(14, 20, setStandDeskHeight);
      setupAlarm(14, 45, setSitDeskHeight);
      
      setupAlarm(15, 25, setStandDeskHeight);
      setupAlarm(15, 50, setSitDeskHeight);
      setupAlarm(16, 15, setStandDeskHeight);
      setupAlarm(16, 40, setSitDeskHeight);
    } else {
      digitalClockDisplay();
      Serial.println("Time updated");
    }
  }
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(year());
  Serial.print("-");
  printDigits(month());
  Serial.print("-");
  printDigits(day());
  Serial.print(" ");
  printDigits(hour());
  Serial.print(":");
  printDigits(minute());
  Serial.print(":");
  printDigits(second());
  Serial.print(" ");
}

void printDigits(int digits) {
  if(digits < 10) {
    Serial.print('0');
  }
  Serial.print(digits);
}


void setupDebouncer(Bounce& debouncer, const int& pin) {
  pinMode(pin, INPUT_PULLUP);
  debouncer.attach(pin);
  debouncer.interval(5);
}

void stopDesk() {
  if (controller.isDriving()) {
    digitalClockDisplay();
    Serial.print("Stopping at height ");
    Serial.print(controller.getCurrentHeight(), 3);
    Serial.println(" m");
    
    controller.stopDrive();
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, controller.getCurrentHeight());
  }
}

void setDeskHeight(const double& height) {
  digitalClockDisplay();
  Serial.print("Setting new height ");
  Serial.print(height, 3);
  Serial.println(" m");
  
  controller.setHeight(height);
  
  if (controller.isEnabled()) {
    //double savedHeight;
    //const int ign = EEPROM_readAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, savedHeight);
    
    if (!controller.isAtTargetHeight()) {
      EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, height);
    }
  }
}

void setMinDeskHeight() { setDeskHeight(controllerParams.minHeight); }

void setSitDeskHeight() { setDeskHeight(sitHeight); }

void setStandDeskHeight() { setDeskHeight(standHeight); }

void setMaxDeskHeight() { setDeskHeight(controllerParams.maxHeight); }

