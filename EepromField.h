#ifndef EepromField_h
#define EepromField_h

#include <Arduino.h>
#include <EEPROM.h>


template<class T>
struct EEPROMFieldParams {
  static const unsigned int size;
  
  unsigned int address;
  
  boolean autoRefresh;
  boolean autoStore;
  
  boolean treat255AsEmpty;
  
  EEPROMFieldParams();
  EEPROMFieldParams(const unsigned int& address);
  EEPROMFieldParams(const unsigned int& address, const boolean& autoRefresh, const boolean& autoStore, const boolean& treat255AsEmpty);
};


template<class T>
class EEPROMField {
public:
  static const T emptyValue;
  
  const EEPROMFieldParams<T> params;
  
  EEPROMField(EEPROMFieldParams<T> params);
  EEPROMField(EEPROMFieldParams<T> params, const T& defaultValue);
  
  boolean isEmpty() const;
  boolean isDirty() const;
  
  operator T() const;
  EEPROMField<T>& operator =(const T& x);
  void reset();
  
  void refresh() const;
  void store() const;
  
private:
  static T getEmptyValue();

  mutable T storedValue;
  T latestValue;
};

#endif

