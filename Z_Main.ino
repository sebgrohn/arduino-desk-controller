
const String TIME_HEADER  = "T";  // header tag for serial time sync message
const char   TIME_REQUEST = 7;    // ASCII bell character requests a time sync message

const char lcdNumCols = 16;
const char lcdNumRows = 2;

const byte DRIVE_DOWN_CHAR = 0;
const byte DRIVE_UP_CHAR   = 1;
const byte DRIVE_STOP_CHAR = 2;
const byte DRIVE_OK_CHAR   = 3;
byte DRIVE_DOWN_CHAR_DEFINITION[8] = {
  B00000,
  B10000,
  B01001,
  B00101,
  B00011,
  B01111,
  B00000,
};
byte DRIVE_UP_CHAR_DEFINITION[8] = {
  B00000,
  B01111,
  B00011,
  B00101,
  B01001,
  B10000,
  B00000,
};
byte DRIVE_STOP_CHAR_DEFINITION[8] = {
  B00000,
  B01110,
  B10011,
  B10101,
  B11001,
  B01110,
  B00000,
};
byte DRIVE_OK_CHAR_DEFINITION[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000,
};

const char CURRENT_HEIGHT_EEPROM_ADDRESS = 0;
const char TARGET_HEIGHT_EEPROM_ADDRESS  = CURRENT_HEIGHT_EEPROM_ADDRESS + sizeof(double);
const char ENABLED_EEPROM_ADDRESS        = TARGET_HEIGHT_EEPROM_ADDRESS + sizeof(double);

Bounce enableDebouncer = Bounce();
Bounce upDebouncer     = Bounce();
Bounce downDebouncer   = Bounce();

EEPROMField<double>  eepromCurrentHeight(EEPROMField<double>::Params(CURRENT_HEIGHT_EEPROM_ADDRESS), sitHeight);
EEPROMField<double>  eepromTargetHeight(EEPROMField<double>::Params(TARGET_HEIGHT_EEPROM_ADDRESS), eepromCurrentHeight);
EEPROMField<boolean> eepromEnabled(EEPROMField<boolean>::Params(ENABLED_EEPROM_ADDRESS), true);

PositionDeskController<4>::Params controllerParams(
    upControlPin, downControlPin,
    minHeight, maxHeight, upSpeed, downSpeed);
PositionDeskController<4> controller(controllerParams, eepromCurrentHeight, eepromTargetHeight);

LiquidCrystal lcd(lcdRSPin, lcdEnablePin, lcdDataPins[0], lcdDataPins[1], lcdDataPins[2], lcdDataPins[3]);

boolean serialInitialized = false;

void setupAlarm(const int& hours, const int& minutes, void (*function)()) {
  const boolean success = (Alarm.alarmRepeat(hours, minutes, 0, function) != dtINVALID_ALARM_ID);
  
  Serial.print('\t');
  printDigits(Serial, hours);
  Serial.print(':');
  printDigits(Serial, minutes);
  Serial.print(' ');
  Serial.print(function != setStandDeskPosition ? (function != setSitDeskPosition ? '?' : 'v') : '^');
  Serial.print(success ? F("") : F(" FAILED"));
}


void setup()  {
  Serial.begin(57600);
  
  setupDebouncer(enableDebouncer, enableInputPin);
  setupDebouncer(upDebouncer, upInputPin);
  setupDebouncer(downDebouncer, downInputPin);
  
  lcd.createChar(DRIVE_DOWN_CHAR, DRIVE_DOWN_CHAR_DEFINITION);
  lcd.createChar(DRIVE_UP_CHAR,   DRIVE_UP_CHAR_DEFINITION);
  lcd.createChar(DRIVE_STOP_CHAR, DRIVE_STOP_CHAR_DEFINITION);
  lcd.createChar(DRIVE_OK_CHAR,   DRIVE_OK_CHAR_DEFINITION);
  lcd.begin(lcdNumCols, lcdNumRows);
  lcd.clear();
  
  controller.params.insertPosition(minPosition,   minHeight);
  controller.params.insertPosition(sitPosition,   sitHeight);
  controller.params.insertPosition(standPosition, standHeight);
  controller.params.insertPosition(maxPosition,   maxHeight);
  controller.setEnabled(eepromEnabled);
}

void loop()  {
  if (!serialInitialized) {
    if (Serial) {
      serialInitialized = true;
      
      printDateTime(Serial);
      Serial.print(F(" Initial height:        "));
      printHeight(Serial, controller.isAtTargetPosition() ? controller.getTargetHeight() : controller.getCurrentHeight(), controller.params);
      Serial.println();
      printDateTime(Serial);
      Serial.print(F(" Initial target height: "));
      printHeight(Serial, controller.getTargetHeight(), controller.params);
      Serial.println();
      printDateTime(Serial);
      Serial.println(controller.isEnabled() ? F(" Enabled") : F(" Disabled"));
    
      setSyncProvider(requestSync);  //set function to call when sync required
      printDateTime(Serial);
      Serial.println(F(" Waiting for time sync..."));
    }
  } else {
    if (Serial.available()) {
      processSyncMessage();
    }
  }
  
  const boolean enableChanged = enableDebouncer.update();
  const boolean upChanged     = upDebouncer.update();
  const boolean downChanged   = downDebouncer.update();
  
  const boolean reachedTargetHeight = controller.update();
  
  const boolean toggleEnable = (enableDebouncer.read() == HIGH);
  const boolean up           = (upDebouncer.read()     == LOW);
  const boolean down         = (downDebouncer.read()   == LOW);
  
  if (enableChanged && toggleEnable) {
    controller.toggleEnabled();
    
    const boolean enabled = controller.isEnabled();
    eepromEnabled = enabled;
    
    printDateTime(Serial);
    Serial.println(enabled ? F(" Enabled") : F(" Disabled"));
    
    if (enabled && !controller.isAtTargetPosition() && Alarm.getNextTrigger() - now() > 10) {
      Alarm.timerOnce(10, resumeDeskDriving);
    }
  }
  
  if (upChanged && up) {
    if (controller.isDrivingDown()) {
      stopDesk();
    } else if (controller.wantToDriveDown()) {
      abortDeskDrive();
    } else {
      raiseDeskPosition();
    }
  }
  
  if (downChanged && down) {
    if (controller.isDrivingUp()) {
      stopDesk();
    } else if (controller.wantToDriveUp()) {
      abortDeskDrive();
    } else {
      lowerDeskPosition();
    }
  }
  
  if (reachedTargetHeight) {
    const double targetHeight = controller.getTargetHeight();
    
    printDateTime(Serial);
    Serial.print(F(" Reached height:     "));
    printHeight(Serial, targetHeight, controller.params);
    Serial.println();
    
    eepromCurrentHeight = targetHeight;
  }
  
  refreshDisplay(lcd);
  
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
      Serial.println(F(" Time synced, setting alarms:"));
      
      // 25 stå, 25-35 sitta
      setupAlarm( 8, 25, setSitDeskPosition);    // 25
      setupAlarm(10, 25, setSitDeskPosition);    // 25
      setupAlarm(13, 10, setSitDeskPosition);    // 35
      setupAlarm(15, 25, setSitDeskPosition);    // 35
      Serial.println();
      
      setupAlarm( 8, 50, setStandDeskPosition);  // 25
      setupAlarm(10, 50, setStandDeskPosition);  // 25
      setupAlarm(13, 45, setStandDeskPosition);  // 25
      setupAlarm(16,  0, setStandDeskPosition);  // 25
      Serial.println();
      
      setupAlarm( 9, 15, setSitDeskPosition);    // 30
      setupAlarm(11, 15, setSitDeskPosition);    // 30 + 1h lunch
      setupAlarm(14, 10, setSitDeskPosition);    // 35
      setupAlarm(16, 25, setSitDeskPosition);    // 35
      Serial.println();
      
      setupAlarm( 9, 45, setStandDeskPosition);  // 25 + 15 fika
      setupAlarm(12, 45, setStandDeskPosition);  // 25
      setupAlarm(14, 45, setStandDeskPosition);  // 25 + 15 fika
      setupAlarm(17,  0, setStandDeskPosition);  // 
      Serial.println();
    } else {
      printDateTime(Serial);
      Serial.println(F(" Time synced"));
    }
  }
}

void setupDebouncer(Bounce& debouncer, const int& pin) {
  pinMode(pin, INPUT_PULLUP);
  debouncer.attach(pin);
  debouncer.interval(5);
}

void stopDesk() {
  controller.stop();
  
  const double currentHeight = controller.getCurrentHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Stopped at height:  "));
  printHeight(Serial, currentHeight, controller.params);
  Serial.println();
  
  eepromCurrentHeight = currentHeight;
}
void abortDeskDrive() {
  const double currentHeight = controller.getCurrentHeight();
  controller.setTargetHeight(currentHeight);
  
  printDateTime(Serial);
  Serial.print(F(" Drove to height:    "));
  printHeight(Serial, currentHeight, controller.params);
  Serial.println();
  
  eepromCurrentHeight = currentHeight;
}
void resumeDeskDriving() {
  controller.resume();
  
  printDateTime(Serial);
  Serial.print(F(" Driving to height:  "));
  printHeight(Serial, controller.getTargetHeight(), controller.params);
  Serial.println();
}
void setDeskPosition(const String& targetPosition) {
  controller.setTargetPosition(targetPosition);
  
  const double targetHeight = controller.getTargetHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Driving to height:  "));
  printHeight(Serial, targetHeight, controller.params);
  Serial.println();
  
  eepromTargetHeight = targetHeight;
}
void raiseDeskPosition() {
  controller.raisePosition();
  
  const double targetHeight = controller.getTargetHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Raising to height:  "));
  printHeight(Serial, targetHeight, controller.params);
  Serial.println();
  
  eepromTargetHeight = targetHeight;
}
void lowerDeskPosition() {
  controller.lowerPosition();
  
  const double targetHeight = controller.getTargetHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Lowering to height: "));
  printHeight(Serial, targetHeight, controller.params);
  Serial.println();
  
  eepromTargetHeight = targetHeight;
}
void setSitDeskPosition()   { setDeskPosition(sitPosition); }
void setStandDeskPosition() { setDeskPosition(standPosition); }


void refreshDisplay(LiquidCrystal& lcd) {
  const time_t currentTime = now();
  
  // current time
  lcd.setCursor(0, 0);
  printTimeShort(lcd);
  
  // enabled/disabled indicator
  lcd.setCursor(8, 0);
  lcd.write(controller.isEnabled() ? DRIVE_OK_CHAR : DRIVE_STOP_CHAR);
  
  // next alarm time
  if (timeStatus() != timeNotSet) {
    lcd.setCursor(10, 0);
    printTimeInterval(lcd, Alarm.getNextTrigger() - currentTime);
  } else {
    lcd.setCursor(10, 0);
    lcd.print(F("      "));
  }
  
  const double currentHeight = controller.getCurrentHeight();
  const double targetHeight  = controller.getTargetHeight();
  
  // current height, driving direction, target height
  if (controller.isAtTargetPosition()) {
    lcd.setCursor(0, 1);
    printHeight(lcd, targetHeight, controller.params);
    
    lcd.setCursor(8, 1);
    lcd.print(F("        "));
  } else {
    // current height
    lcd.setCursor(0, 1);
    printHeight(lcd, currentHeight, controller.params);
    
    // driving direction
    lcd.setCursor(8, 1);
    if (targetHeight > currentHeight) {
      lcd.write(DRIVE_UP_CHAR);
    } else {
      lcd.write(DRIVE_DOWN_CHAR);
    }
    
    // target height
    lcd.setCursor(10, 1);
    printHeight(lcd, targetHeight, controller.params);
  }
}
