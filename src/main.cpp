#include <Wire.h>

#include <Arduino.h>

#include "Constants.hpp"
#include "functions/altRead.hpp"
#include "functions/blink.hpp"
#include "functions/eeprom.hpp"
#include "functions/print.hpp"
#include "functions/Detect.hpp"


// Use classes or structs to group related parameters together

// instead of top declaring everything as global functions
// use header files.


// try to declare less global variables. Scope your variables
// to your main looping function or in the function where they are
// needed. This ensures better readibility and maintainability



int butt = 0; // Button state


unsigned long gtime; // General Time variable
int ltime = 0; // LED time variable

int state[5]; // Array for storing the state of the rocket

bool logging = false; // Logging detection

void setup() {
  // put your setup code here, to run once:
  Serial.begin(serialBaud);
  pinMode(LED, OUTPUT);
  delay(1000);
  while (!Serial) delay(100);

  led(100, 3);

  bmp_temp = bmp.getTemperatureSensor();
  bmp_pressure = bmp.getPressureSensor();
  
  unsigned status;
  status = bmp.begin();
  if (!status) {
    digitalWrite(LED, HIGH);
    while (1);
  } else {
    led(200, 2);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  delay(200);
  altZero();

  gtime = millis(); // Initialize gtime with millis()

  adr = 4; // Initialize adr with 4

  if (readFloatFromEEPROM(0) != 0) {
    logging = true;
    apg = readFloatFromEEPROM(0);
    Serial.println("Previous flight detected");
    Serial.print("Apogee: ");
    Serial.println(apg);
    led(75, 3);
  }

}


// Don't have void functions that work on global variables
// Use parameters and return values instead
// This makes your code easier to understand and maintain
// if this is your main loop, label it as such
void loop() {

  if (logging) {
    print();
  }

  // if your main loop function has its own delay, in a sub-function this 
  // wrecks your iteration rate. Fix this
  if (landDetect() == true) {
    led(100, 4);
    split(apg);
    while(1) {
      pulse();
    }
  }

  unsigned long curtime = millis();
  float elapsed = (curtime - gtime) / 1000.0;
  tadr = adr + 4;

  writeFloatToEEPROM(adr, altRead(10, zero));
  delay(10);
  writeFloatToEEPROM(tadr, elapsed);

  apgDetect();
  landDetect();

  Serial.print(readFloatFromEEPROM(tadr));
  Serial.print(", ");
  Serial.print(adr);
  Serial.print(", ");
  Serial.println(readFloatFromEEPROM(adr));

  // if (analogRead(but) > 500) {
  //   apogee = true;
  //   apg = 2564;
  //   delay(20);
  //   writeFloatToEEPROM(0, apg);
  //   Serial.println("Simulated apogee");
  //   led(200, 3);
  // }

  adr += 8;
  led(20, 1);
}