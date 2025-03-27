#ifndef ALTREAD_HPP
#define ALTREAD_HPP

#include <Adafruit_BMP280.h>


Adafruit_BMP280 bmp;

Adafruit_Sensor *bmp_temp;
Adafruit_Sensor *bmp_pressure;

float altRead(int amt, float zero) {
    float Average[10];
    float alt = 0;
    for (int i = 0; i < amt; i++) {
      Average[i] = (bmp.readAltitude(1013.25) * 3.28 - zero);
      alt += Average[i];
    }
    alt /= amt;
    return alt;
  }

#endif