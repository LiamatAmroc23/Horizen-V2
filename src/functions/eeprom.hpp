#ifndef EEPROM_H
#define EEPROM_H

#include <at24c256.h>

AT24C256 eeprom(0x50);

int adr = 4; // eeprom address
int tadr = 8; // time eeprom address

float readFloatFromEEPROM(int address) {
    float value;
    byte data[4];
    eeprom.read(address, data, 4); // Read 4 bytes from EEPROM

    memcpy(&value, data, sizeof(value));
    return value;
  }
  
  void writeFloatToEEPROM(int address, float value) {
    byte data[4];
    if (address + 4 > 255000) { // Assuming 32KB EEPROM
      Serial.println("Error: EEPROM address out of bounds");
      return; // Don't write beyond the end of EEPROM
    }
    memcpy(data, &value, sizeof(value));
    eeprom.write(address, data, 4); // Write 4 bytes to EEPROM
  }
  
  void clearEEPROM() {
    byte zeros[32] = {0}; // Buffer of 32 zeros
    for (int i = 0; i < 100000; i += 32) { 
      eeprom.write(i, zeros, 32); // Write 32 zeros at a time
       // Yield to prevent watchdog timer reset
       Serial.println(i);
       if (readFloatFromEEPROM(i) == 0)
       {
        break;
       }
       
       delay(10);
    }
  }
  #endif // EEPROM_H