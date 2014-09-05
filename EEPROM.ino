
int EEPROM_writeAnything(int ee, const double& value) {
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) {
    EEPROM.write(ee++, *p++);
  }
  return i;
}

int EEPROM_readAnything(int ee, double& value) {
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++) {
    *p++ = EEPROM.read(ee++);
  }
  return i;
}

