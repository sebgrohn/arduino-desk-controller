#ifndef Config_h
#define Config_h

#include <Arduino.h>


const double minHeight   = 0.690;  // in meters
const double maxHeight   = 1.175;  // in meters  // TODO measure it
const double sitHeight   = 0.700;  // in meters
const double standHeight = 1.110;  // in meters

const double upSpeed   = 0.02050;  // in m/s
const double downSpeed = 0.02300;  // in m/s

const String minPosition   = "Min   ";
const String maxPosition   = "Max   ";
const String sitPosition   = "Sit   ";
const String standPosition = "Stand ";

const char enableInputPin = 0;
const char upInputPin     = 5;
const char downInputPin   = 4;

const char lcdRSPin      = 8;
const char lcdEnablePin  = 9;
const char lcdDataPins[] = { 10, 11, 12, 13 };  // data pin 4-7

const char upControlPin   = 7;
const char downControlPin = 6;

#endif

