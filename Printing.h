#ifndef Printing_h
#define Printing_h

#include <Arduino.h>


void printDateTime(Print& printer, const time_t& time);
void printDateTime(Print& printer);

void printTimeShort(Print& printer, const time_t& time);
void printTimeShort(Print& printer);

void printTimeInterval(Print& printer, const time_t& timeInterval);

void printLength(Print& printer, const double& length);
void printHeight(Print& printer, const double& height, const PositionDeskControllerParams& params);

void printDigits(Print& printer, const int& digits);

#endif

