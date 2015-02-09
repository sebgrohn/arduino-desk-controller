
const double minHeight   = 0.685;  // in meters
const double maxHeight   = 1.175;  // in meters
const double sitHeight   = 0.700;  // in meters
const double standHeight = 1.110;  // in meters

const char enableInputPin = 4;
const char upInputPin     = 3;
const char downInputPin   = 2;

const char lcdRSPin      = 8;
const char lcdEnablePin  = 9;
const char lcdDataPins[] = { 10, 11, 12, 13 };  // data pin 4-7

const char upControlPin   = 7;
const char downControlPin = 6;
const char statusLedPin   = 5;

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

LiquidCrystal lcd(lcdRSPin, lcdEnablePin, lcdDataPins[0], lcdDataPins[1], lcdDataPins[2], lcdDataPins[3]);

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
  printHeight(Serial, initialHeight);
  Serial.println();
  
  setSyncProvider(requestSync);  //set function to call when sync required
  printDateTime(Serial);
  Serial.println(" Waiting for time sync...");
  
  setupDebouncer(enableDebouncer, enableInputPin);
  setupDebouncer(upDebouncer, upInputPin);
  setupDebouncer(downDebouncer, downInputPin);
  
  pinMode(statusLedPin, OUTPUT);
  
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
      printHeight(Serial, targetHeight);
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
  
  digitalWrite(statusLedPin, controller.isDriving() || upDebouncer.read() == LOW || downDebouncer.read() == LOW ? HIGH : LOW);
  
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

void printDateTime(Print& printer, const time_t& time) {
  if (time != dtINVALID_TIME) {
    printer.print(year(time));
    printer.print("-");
    printDigits(printer, month(time));
    printer.print("-");
    printDigits(printer, day(time));
    printer.print(" ");
    printDigits(printer, hour(time));
    printer.print(":");
    printDigits(printer, minute(time));
    printer.print(":");
    printDigits(printer, second(time));
  } else {
    printer.print("                   ");
  }
}
void printDateTime(Print& printer) {
  if (timeStatus() != timeNotSet) {
    printDateTime(printer, now());
  } else {
    printer.print("xxxx-xx-xx --:--:--");
  }
}
void printTimeShort(Print& printer, const time_t& time) {
  if (time != dtINVALID_TIME) {
    printDigits(printer, hour(time));
    printer.print(":");
    printDigits(printer, minute(time));
  } else {
    printer.print("     ");
  }
}
void printTimeShort(Print& printer) {
  if (timeStatus() != timeNotSet) {
    printTimeShort(printer, now());
  } else {
    printer.print("--:--");
  }
}
void printDigits(Print& printer, int digits) {
  if(digits < 10) {
    printer.print('0');
  }
  printer.print(digits);
}

void printLength(Print& printer, const double& length) {
  if (!isnan(length)) {
    printer.print(length, 2);
    printer.print(" m");
  } else {
    printer.print("---- m");
  }
}
void printHeight(Print& printer, const double& height) {
  if (height == sitHeight) {
    printer.print("Sit   ");
  } else if (height == standHeight) {
    printer.print("Stand ");
  } else if (height == controllerParams.minHeight) {
    printer.print("Min   ");
  } else if (height == controllerParams.maxHeight) {
    printer.print("Max   ");
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
  printDateTime(Serial);
  Serial.print(" Stopped at: ");
  printHeight(Serial, controller.getCurrentHeight());
  Serial.println();
  
  controller.stopDrive();
  
  if (controller.isEnabled() && !controller.isAtTargetHeight()) {
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, controller.getCurrentHeight());
  }
}

void setDeskHeight(const double& targetHeight) {
  printDateTime(Serial);
  Serial.print(" Driving to: ");
  printHeight(Serial, targetHeight);
  Serial.println();
  
  controller.setHeight(targetHeight);
  
  if (controller.isEnabled() && !controller.isAtTargetHeight()) {
    printDateTime(Serial);
    Serial.print(" Saving height: ");
    printHeight(Serial, targetHeight);
    Serial.println();
    
    EEPROM_writeAnything(CURRENT_HEIGHT_EEPROM_ADDRESS, targetHeight);
  }
}
void setSitDeskHeight()   { setDeskHeight(sitHeight); }
void setStandDeskHeight() { setDeskHeight(standHeight); }


void refreshDisplay(LiquidCrystal& lcd) {
  // current time
  lcd.setCursor(0, 0);
  printTimeShort(lcd);
  
  // enabled/disabled indicator
  lcd.setCursor(15, 0);
  lcd.write(controller.isEnabled() ? DRIVE_OK_CHAR : DRIVE_STOP_CHAR);
  
  // next alarm time
  if (timeStatus() != timeNotSet) {
    lcd.setCursor(9, 0);
    printTimeShort(lcd, Alarm.getNextTrigger());
  } else {
    lcd.setCursor(9, 0);
    lcd.print("     ");
  }
  
  // current height, driving direction, target height
  if (controller.isAtTargetHeight()) {
    lcd.setCursor(0, 1);
    printHeight(lcd, controller.getTargetHeight());
    
    lcd.setCursor(7, 1);
    lcd.print("        ");
  } else {
    // current height
    lcd.setCursor(0, 1);
    printHeight(lcd, controller.getCurrentHeight());
    
    // driving direction
    lcd.setCursor(7, 1);
    switch(controller.getDrivingDirection()) {
    case UP:
      lcd.write(DRIVE_UP_CHAR);
      break;
    
    case NONE:
      lcd.write(DRIVE_STOP_CHAR);
      break;
    
    case DOWN:
      lcd.write(DRIVE_DOWN_CHAR);
      break;
    }
    
    // target height
    lcd.setCursor(9, 1);
    printHeight(lcd, controller.getTargetHeight());
  }
}
