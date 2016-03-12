#ifndef Printing_h
#define Printing_h

#include <Arduino.h>


void printDateTime(Print& printer, const time_t& time);
void printDateTime(Print& printer);

void printTimeShort(Print& printer, const time_t& time);
void printTimeShort(Print& printer);

void printTimeInterval(Print& printer, const time_t& timeInterval);

void printLength(Print& printer, const double& length);

template<size_t N>
void printHeight(Print& printer, const double& height, const PositionDeskControllerParams<N>& params) {
  const Position position = params.getPosition(height);
  if (position.first != String()) {
    printer.print(position.first);
  } else {
    printLength(printer, height);
  }
}

void printDigits(Print& printer, const int& digits);

#endif

