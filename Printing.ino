
#include "Printing.h"

#include <Time.h>

#include "PositionDeskController.h"


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
    printer.print(F(" days"));
  } else if (hours > 8) {
    printer.print(hours + 1);
    printer.print(F(" h  "));
  } else if (hours > 0) {
    printer.print(mins == 59 ? hours + 1 : hours);
    printer.print(F(":"));
    printDigits(printer, (mins + 1) % 60);
    printer.print(F(" h"));
  } else if (mins > 0) {
    printer.print(mins + 1);
    printer.print(F(" min "));
  } else if (secs > 0) {
    printer.print(secs + 1);
    printer.print(F(" s   "));
  }
}


void printLength(Print& printer, const double& length) {
  if (!isnan(length)) {
    printer.print(length, 2);
    printer.print(F(" m"));
  } else {
    printer.print(F("---- m"));
  }
}


void printDigits(Print& printer, const int& digits) {
  if(digits < 10) {
    printer.print('0');
  }
  printer.print(digits);
}

