
const double minHeight   = 0.685;  // in meters
const double maxHeight   = 1.175;  // in meters
const double sitHeight   = 0.700;  // in meters
const double standHeight = 1.110;  // in meters

const String minPosition   = "Min   ";
const String maxPosition   = "Max   ";
const String sitPosition   = "Sit   ";
const String standPosition = "Stand ";

const char enableInputPin = 4;
const char upInputPin     = 3;
const char downInputPin   = 2;

const char lcdRSPin      = 8;
const char lcdEnablePin  = 9;
const char lcdDataPins[] = { 10, 11, 12, 13 };  // data pin 4-7

const char upControlPin   = 7;
const char downControlPin = 6;

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

const char CURRENT_HEIGHT_EEPROM_ADDRESS        = 0;
const char CURRENT_TARGET_HEIGHT_EEPROM_ADDRESS = CURRENT_HEIGHT_EEPROM_ADDRESS + sizeof(double);

Bounce enableDebouncer = Bounce();
Bounce upDebouncer     = Bounce();
Bounce downDebouncer   = Bounce();

double initialHeight;
double initialTargetHeight;
const int ignored1 = EEPROM_readAnything(CURRENT_HEIGHT_EEPROM_ADDRESS,        initialHeight);
const int ignored2 = EEPROM_readAnything(CURRENT_TARGET_HEIGHT_EEPROM_ADDRESS, initialTargetHeight);

std::map<String, double> createControllerPositions() {
  std::map<String, double> controllerPositions = std::map<String, double>();
  controllerPositions[minPosition]   = minHeight;
  controllerPositions[maxPosition]   = maxHeight;
  controllerPositions[sitPosition]   = sitHeight;
  controllerPositions[standPosition] = standHeight;
  return controllerPositions;
}
PositionDeskControllerParams controllerParams(
    upControlPin, downControlPin,
    minHeight, maxHeight, 0.0295, 0.0335,
    createControllerPositions());
PositionDeskController controller(controllerParams, !isnan(initialHeight) ? initialHeight : minHeight, !isnan(initialTargetHeight) ? initialTargetHeight : (!isnan(initialHeight) ? initialHeight : minHeight));

LiquidCrystal lcd(lcdRSPin, lcdEnablePin, lcdDataPins[0], lcdDataPins[1], lcdDataPins[2], lcdDataPins[3]);

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
  
  printDateTime(Serial);
  Serial.print(F(" Initial height:        "));
  printHeight(Serial, controller.isAtTargetPosition() ? controller.getTargetHeight() : controller.getCurrentHeight());
  Serial.println();
  printDateTime(Serial);
  Serial.print(F(" Initial target height: "));
  printHeight(Serial, controller.getTargetHeight());
  Serial.println();
  
  setSyncProvider(requestSync);  //set function to call when sync required
  printDateTime(Serial);
  Serial.println(F(" Waiting for time sync..."));
  
  setupDebouncer(enableDebouncer, enableInputPin);
  setupDebouncer(upDebouncer, upInputPin);
  setupDebouncer(downDebouncer, downInputPin);
  
  lcd.createChar(DRIVE_DOWN_CHAR, DRIVE_DOWN_CHAR_DEFINITION);
  lcd.createChar(DRIVE_UP_CHAR,   DRIVE_UP_CHAR_DEFINITION);
  lcd.createChar(DRIVE_STOP_CHAR, DRIVE_STOP_CHAR_DEFINITION);
  lcd.createChar(DRIVE_OK_CHAR,   DRIVE_OK_CHAR_DEFINITION);
  lcd.begin(lcdNumCols, lcdNumRows);
  lcd.clear();
  
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
  const boolean up     = (upDebouncer.read()     == LOW);
  const boolean down   = (downDebouncer.read()   == LOW);
  
  if (enableChanged) {
    controller.setEnabled(enable);
    
    printDateTime(Serial);
    Serial.println(enable ? F(" Enabled") : F(" Disabled"));
  }
  
  if (upChanged && up) {
    switch (controller.getDrivingDirection()) {
    case DOWN:
      stopDesk();
      break;
      
    default:
      raiseDeskPosition();
      break;
    }
  }
  
  if (downChanged && down) {
    switch (controller.getDrivingDirection()) {
    case UP:
      stopDesk();
      break;
      
    default:
      lowerDeskPosition();
      break;
    }
  }
  
  if (reachedTargetHeight) {
    const double targetHeight = controller.getTargetHeight();
    
    printDateTime(Serial);
    Serial.print(F(" Reached height:        "));
    printHeight(Serial, targetHeight);
    Serial.println();
    
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, targetHeight);
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

void printDateTime(Print& printer, const time_t& time) {
  if (time != dtINVALID_TIME) {
    printer.print(year(time));
    printer.print('-');
    printDigits(printer, month(time));
    printer.print('-');
    printDigits(printer, day(time));
    printer.print(' ');
    printDigits(printer, hour(time));
    printer.print(':');
    printDigits(printer, minute(time));
    printer.print(':');
    printDigits(printer, second(time));
  } else {
    printer.print(F("                   "));
  }
}
void printDateTime(Print& printer) {
  if (timeStatus() != timeNotSet) {
    printDateTime(printer, now());
  } else {
    printer.print(F("xxxx-xx-xx --:--:--"));
  }
}
void printTimeShort(Print& printer, const time_t& time) {
  if (time != dtINVALID_TIME) {
    printDigits(printer, hour(time));
    printer.print(':');
    printDigits(printer, minute(time));
  } else {
    printer.print(F("     "));
  }
}
void printTimeShort(Print& printer) {
  if (timeStatus() != timeNotSet) {
    printTimeShort(printer, now());
  } else {
    printer.print(F("--:--"));
  }
}
void printTimeInterval(Print& printer, const time_t& timeInterval) {
  time_t timeIntervalRem = timeInterval;
  const int days  = timeIntervalRem / 86400; timeIntervalRem -= days  * 86400;
  const int hours = timeIntervalRem / 3600;  timeIntervalRem -= hours * 3600;
  const int mins  = timeIntervalRem / 60;    timeIntervalRem -= mins  * 60;
  const int secs  = timeIntervalRem;
  
  if (days > 0) {
    printer.print(days + 1);
    printer.print(F(" d   "));
  } else if (hours > 10) {
    printer.print(hours + 1);
    printer.print(F(" h  "));
  } else if (hours > 0) {
    printer.print(mins == 59 ? hours + 1 : hours);
    printer.print(F(":"));
    printDigits(printer, (mins + 1) % 60);
    printer.print(F(" h"));
  } else if (mins > 0) {
    printer.print(mins + 1);
    printer.print(F(" m   "));
  } else if (secs > 30) {
    printer.print(F("1 m   "));
  } else if (secs > 10) {
    printer.print(F("30 s  "));
  } else if (secs > 0) {
    printer.print(secs);
    printer.print(F(" s   "));
  }
}
void printDigits(Print& printer, const int& digits) {
  if(digits < 10) {
    printer.print('0');
  }
  printer.print(digits);
}

void printLength(Print& printer, const double& length) {
  if (!isnan(length)) {
    printer.print(length, 2);
    printer.print(F(" m"));
  } else {
    printer.print(F("---- m"));
  }
}
void printHeight(Print& printer, const double& height) {
  const PositionDeskControllerParams::position position = controller.params.getPosition(height);
  if (position.first != String()) {
    printer.print(position.first);
  } else {
    printLength(printer, height);
  }
}

void setupDebouncer(Bounce& debouncer, const int& pin) {
  pinMode(pin, INPUT_PULLUP);
  debouncer.attach(pin);
  debouncer.interval(5);
}

void stopDesk() {
  controller.stopDrive();
  
  const double currentHeight = controller.getCurrentHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Stopped at height:     "));
  printHeight(Serial, currentHeight);
  Serial.println();
  
  if (controller.isEnabled() && !controller.isAtTargetPosition()) {
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, currentHeight);
  }
}

void setDeskPosition(const String& targetPosition) {
  controller.setPosition(targetPosition);
  
  const double targetHeight = controller.getTargetHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Driving to height:     "));
  printHeight(Serial, targetHeight);
  Serial.println();
  
  if (!controller.isAtTargetPosition()) {
    EEPROM_writeAnything(CURRENT_TARGET_HEIGHT_EEPROM_ADDRESS, targetHeight);
  }
}
void raiseDeskPosition() {
  controller.raisePosition();
  
  const double targetHeight = controller.getTargetHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Raising to height:     "));
  printHeight(Serial, targetHeight);
  Serial.println();
  
  if (!controller.isAtTargetPosition()) {
    EEPROM_writeAnything(CURRENT_TARGET_HEIGHT_EEPROM_ADDRESS, targetHeight);
  }
}
void lowerDeskPosition() {
  controller.lowerPosition();
  
  const double targetHeight = controller.getTargetHeight();
  
  printDateTime(Serial);
  Serial.print(F(" Lowering to height:    "));
  printHeight(Serial, targetHeight);
  Serial.println();
  
  if (!controller.isAtTargetPosition()) {
    EEPROM_writeAnything(CURRENT_TARGET_HEIGHT_EEPROM_ADDRESS, targetHeight);
  }
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
    printHeight(lcd, targetHeight);
    
    lcd.setCursor(7, 1);
    lcd.print(F("         "));
  } else {
    // current height
    lcd.setCursor(0, 1);
    printHeight(lcd, currentHeight);
    
    // driving direction
    lcd.setCursor(7, 1);
    if (targetHeight > currentHeight) {
      lcd.write(DRIVE_UP_CHAR);
    } else {
      lcd.write(DRIVE_DOWN_CHAR);
    }
    lcd.write(controller.isDriving() ? ' ' : DRIVE_STOP_CHAR);
    
    // target height
    lcd.setCursor(10, 1);
    printHeight(lcd, targetHeight);
  }
}
