#ifndef Config_h
#define Config_h

#include <Arduino.h>


const double minHeight   = 0.685;  // in meters
const double maxHeight   = 1.175;  // in meters
const double sitHeight   = 0.700;  // in meters
const double standHeight = 1.110;  // in meters

const double upSpeed   = 0.0295;  // in m/s
const double downSpeed = 0.0335;  // in m/s

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

#endif

