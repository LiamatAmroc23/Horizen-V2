#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <at24c256.h>
#include <Arduino.h>

void led(int del, int amt);
void print();
void writ();
void fstate();
float readFloatFromEEPROM(int address);
float altread();
float avg(int amt);

int i; // general for loop usage
int d;
int dg; // amount of digits in the split int

int zero = 0; // Starting altitude
int cur = 0; // Variables for sensing apogee
int pre = 0;

int adr = 0; // eeprom address

int array[5];
int narray[5];
int state[5]; // Array for storing the state of the rocket

float value;

bool apogee = false; // Apogee detection
bool deploy = false; // Deployment detection
bool land = false; // Landing detection
bool armed = false; // Arming detection
bool logging = false; // Logging detection

const int LED = 14; // LED pin
const int but = 0; // Button pin

const int apgthres = 200; // Altitude at which apogee starts being detected

Adafruit_BMP280 bmp;
AT24C256 eeprom(0x50);

Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getTemperatureSensor();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  while (!Serial) delay(100);

  led(100, 3);

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
  zero = bmp.readAltitude(1013.25) * 3.28;
  float storedValue = readFloatFromEEPROM(0x00); // Replace 0x00 with the address you want to read
  if (storedValue >= 0) {
    logging = true;
  } else {
    logging = false;
  }
}

void led(int del, int amt) {
  int j = 0;
  while (j != amt) {
    digitalWrite(LED, HIGH);
    delay(del);
    digitalWrite(LED, LOW);
    delay(del);
    j += 1;
  }
}

float readFloatFromEEPROM(int address) {
  byte data[4];
  eeprom.read(address, data, 4); // Read 4 bytes from EEPROM
  float value;
  memcpy(&value, data, sizeof(value));
  return value;
}

void split(int num) {
  int va = num;
  int v = 0;
  i = 0;
  dg = 0; // Initialize dg to 0
  while (va != 0) {
    va /= 10;
    dg += 1;
  }
  while (num != 0) {
    array[i] = num % 10;
    num /= 10;
    i += 1;
  }
  d = dg;
  while (d != 0) {
    d -= 1;
    narray[v] = array[d];
    v += 1; // Increment v to fill narray correctly
  }
}

void pulse() {
  int l = 0;
  i = 0;
  while (i != dg) {
    l = 0;
    if (narray[i] == 0) {
      digitalWrite(LED, HIGH);
      delay(550);
      digitalWrite(LED, LOW);
      delay(200);
    } else {
      while (l != narray[i]) {
        led(200, 1);
        l += 1;
      }
    }
    i += 1; // Fix increment issue
    delay(500);
  }
  delay(2000);
}

void print() {
  int k = 4;
  while (but < 500) {
    value = readFloatFromEEPROM(k);
    k += 4;
    Serial.println(value);
    if (value == 0) {
      k = 4;
    }
  }
  while (1);
}

void fstate() {
  float alt = altread();
  if (alt <= 20) {
    // Add your code here
  }
}

float altread() {
  float alt = bmp.readAltitude(1013.25) * 3.28 - zero;
  return alt;
}

float avg(int amt) {
  float avg = 0;
  for (int i = 0; i < amt; i++) {
    array[i] = altread();
    avg += array[i];
  }
  avg /= amt;
  return avg;
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
