#include <Arduino.h>

#include "Constants.hpp"

#include "functions/eeprom.hpp"
#include "functions/altRead.hpp"
#include "functions/blink.hpp"


int apg = 0; // Apogee altitude

int cur = 0; // Variables for sensing apogee
int pre = 0;

int g;
int l;

float zero;

bool apogee = false; // Apogee detection
bool land = false; // Landing detection

float altZero(){
    zero = altRead(10, 0);
    return zero;
}

void apgDetect() {
    cur = altRead(10, zero);
    if (cur > pre && cur > apgthres) {
      apg = cur;
    }
    else if(cur < pre && cur < apg && !apogee) {
      apogee = true;
      writeFloatToEEPROM(0x00, apg); 
      led(200, 3);
    }
    pre = cur;
  }
  
  // Don't have void functions that work on global variables
  // why is 500 hardcoded here?
  bool landDetect() {
    if (g >= 500) {
      // same with cur < 20 
      if (cur < 20 && apogee)
      {
         l += 1;
         // don't hardcode 20
            if(l >= 20) {
            return true;
            }
            else {
              return false;
            }
            // is this delay part of your loop? that is crazy dangerous in a realtime system
            // use a cursor so that you don't arbitrarily delay during your loop
            delay(500);
        
      }
    }
    else {
      delay(1);
      return false;
    }
    return false;
  }

  void print() {
    split(apg);
      while(analogRead(but) < 500) {
          tadr = adr + 4;
  
          Serial.print(readFloatFromEEPROM(tadr));
          Serial.print(", ");
          Serial.println(readFloatFromEEPROM(adr));
            
          adr += 8;   
          led(20, 1); 
      
     }
     led(1000, 1);
     while (analogRead(but) < 500)
     {
        pulse();
     }
     
  
     
      digitalWrite(LED, HIGH);
      delay(20);
      clearEEPROM();
      digitalWrite(LED, LOW);
      while(1);
  }