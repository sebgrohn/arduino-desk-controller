
#include "PositionDeskController.h"
#include "EepromField.h"

#include <Time.h>
#include <TimeAlarms.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>
#include <map>

// ---------------------------------------

struct LiquidCrystalWindowParams {
  
  int x;
  int y;
  
  int width;
  int height;
  
  LiquidCrystalWindowParams();
  
  LiquidCrystalWindowParams(const int& x, const int& y, const int& width, const int& height);
};


class LiquidCrystalWindow : public Print {
public:
  const LiquidCrystalWindowParams params;
  
  LiquidCrystal * const lcd;
  
  LiquidCrystalWindow(LiquidCrystal * const & lcd, const LiquidCrystalWindowParams& params);
  
  virtual size_t write(uint8_t);
};



LiquidCrystalWindowParams::LiquidCrystalWindowParams() {}

LiquidCrystalWindowParams:: LiquidCrystalWindowParams(const int& x, const int& y, const int& width, const int& height)
    : x(x),
      y(y),
      width(width),
      height(height) {
}


LiquidCrystalWindow::LiquidCrystalWindow(LiquidCrystal * const & lcd, const LiquidCrystalWindowParams& params)
    : lcd(lcd),
      params(params) {
}

size_t LiquidCrystalWindow::write(uint8_t value) {
  lcd->setCursor(params.x, params.y);
  return lcd->write(value);
}

