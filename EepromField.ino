
#include "EepromField.h"

#include <EEPROM.h>


template<class T>
const size_t EEPROMFieldParams<T>::size = sizeof(T);

template<class T>
EEPROMFieldParams<T>::EEPROMFieldParams()
    : autoRefresh(true),
      autoStore(true),
      treat255AsEmpty(true) {}

template<class T>
EEPROMFieldParams<T>::EEPROMFieldParams(const unsigned int& address)
    : EEPROMFieldParams(address, true, true, true) {}

template<class T>
EEPROMFieldParams<T>::EEPROMFieldParams(const unsigned int& address, const boolean& autoRefresh, const boolean& autoStore, const boolean& treat255AsEmpty)
    : address(address),
      autoRefresh(autoRefresh),
      autoStore(autoStore),
      treat255AsEmpty(treat255AsEmpty) {}


template<class T>
const T EEPROMField<T>::emptyValue = EEPROMField<T>::getEmptyValue();

template<class T>
T EEPROMField<T>::getEmptyValue() {
  T emptyValue;
  byte const * it = (byte const *)(void const *)&emptyValue;
  for (; it != it + EEPROMFieldParams<T>::size; it++) {
    *it = 255;
  }
  return emptyValue;
}

template<class T>
EEPROMField<T>::EEPROMField(Params params)
    : params(params) {
  refresh();
  latestValue = storedValue;
}

template<class T>
EEPROMField<T>::EEPROMField(Params params, const T& defaultValue)
    : params(params) {
  refresh();
  if (isEmpty()) {
    *this = defaultValue;
  } else {
    latestValue = storedValue;
  }
}

template<class T>
boolean EEPROMField<T>::isEmpty() const {
  if (!params.treat255AsEmpty) {
    return false;
  }
  
  byte const * it = (byte const *)(void const *)&latestValue;
  for (; it != it + EEPROMFieldParams<T>::size; it++) {
    if (*it != 255) {
      return false;
    }
  }
  return true;
}

template<class T>
boolean EEPROMField<T>::isDirty() const {
  return latestValue != storedValue;
}

template<class T>
EEPROMField<T>::operator T() const {
  return latestValue;
}

template<class T>
EEPROMField<T>& EEPROMField<T>::operator =(const T& newValue) {
  latestValue = newValue;
  if (params.autoStore) {
    store();
  }
  return *this;
}

template<class T>
void EEPROMField<T>::reset() {
  *this = emptyValue;
}

template<class T>
void EEPROMField<T>::refresh() const {
  byte * it = (byte*)(void*)&storedValue;
  unsigned int address = params.address;
  
  for (int i = 0; i < EEPROMFieldParams<T>::size; i++) {
    *it++ = EEPROM.read(address++);
  }
}

template<class T>
void EEPROMField<T>::store() const {
  if (params.autoRefresh) {
    refresh();
  }
  
  if (isDirty()) {
    byte const * itStored = (byte const *)(void const *)&storedValue;
    byte const * itLatest = (byte const *)(void const *)&latestValue;
    unsigned int address = params.address;
    
    for (int i = 0; i < EEPROMFieldParams<T>::size; i++) {
      // only write bytes that differ
      if (*itLatest != *itStored) {
        EEPROM.write(address++, *itLatest++);
      }
    }
    
    storedValue = latestValue;
  }
}

