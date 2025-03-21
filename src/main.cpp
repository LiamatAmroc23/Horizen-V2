#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <at24c256.h>
#include <Arduino.h>

void led(int del, int amt);
void print();
void writ();
float altRead(int amt);
void apgDetect();
float readFloatFromEEPROM(int address);
void writeFloatToEEPROM(int address, float value);
void clearEEPROM();

int i; // general for loop usage
int d;
int dg; // amount of digits in the split int

int zero = 0; // Starting altitude
int cur = 0; // Variables for sensing apogee
int pre = 0;

int apg = 0; // Apogee altitude

unsigned long gtime; // General Time variable
int ltime = 0; // LED time variable

int adr = 4; // eeprom address
int tadr = 8; // time eeprom address

int array[5];
float aarray[10];
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

Adafruit_Sensor *bmp_temp;
Adafruit_Sensor *bmp_pressure;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  zero = bmp.readAltitude(1013.25) * 3.28;

  gtime = millis(); // Initialize gtime with millis()

  adr = 4; // Initialize adr with 4
  writeFloatToEEPROM(0, 0); // Write 0 to EEPROM address 0

  if (readFloatFromEEPROM(0) != 0) {
    logging = true;
    apg = readFloatFromEEPROM(0);
    Serial.println("Previous flight detected");
    Serial.print("Apogee: ");
    Serial.println(apg);
  }

  // Clear EEPROM at the start
  clearEEPROM();
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
  Serial.print("Raw data read from EEPROM at address ");
  Serial.print(address);
  Serial.print(": ");
  for (int i = 0; i < 4; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  float value;
  memcpy(&value, data, sizeof(value));
  Serial.print("Read from EEPROM at address ");
  Serial.print(address);
  Serial.print(": ");
  Serial.println(value, 6); // Print with 6 decimal places for precision
  return value;
}

void writeFloatToEEPROM(int address, float value) {
  byte data[4];
  if (address + 4 > 32768) { // Assuming 32KB EEPROM
    Serial.println("Error: EEPROM address out of bounds");
    return; // Don't write beyond the end of EEPROM
  }
  memcpy(data, &value, sizeof(value));
  eeprom.write(address, data, 4); // Write 4 bytes to EEPROM
  Serial.print("Written to EEPROM at address ");
  Serial.print(address);
  Serial.print(": ");
  Serial.println(value, 6); // Print with 6 decimal places for precision
}

void clearEEPROM() {
  byte zeros[32] = {0}; // Buffer of 32 zeros
  for (int i = 0; i < 32768; i += 32) { // Assuming 32KB EEPROM
    eeprom.write(i, zeros, 32); // Write 32 zeros at a time
    yield(); // Yield to prevent watchdog timer reset
  }
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
    Serial.println(narray[i]);
    delay(500);
  }
  delay(2000);
}

void print() {
  int k = 4;
  split(apg);
  while (analogRead(but) < 500) {
    value = readFloatFromEEPROM(k);
    k += 4;
    Serial.println(value);
    if (value == 0) {
      k = 4;
    }
    led(50, 1);
    //pulse();
  }
  while (1) {
    digitalWrite(LED, HIGH);
    delay(20);
    clearEEPROM();
    digitalWrite(LED, LOW);
    while(1);
  }
}

void apgDetect() {
  cur = altRead(10);
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

float altRead(int amt) {
  float alt = 0;
  for (int i = 0; i < amt; i++) {
    aarray[i] = (bmp.readAltitude(1013.25) * 3.28 - zero);
    alt += aarray[i];
  }
  alt /= amt;
  return alt;
}

void loop() {

  if (logging) {
    print();
  }

  unsigned long curtime = millis();
  float elapsed = (curtime - gtime) / 1000.0;
  tadr = adr + 4;

  // Debugging statements
  Serial.print("Writing altitude to EEPROM at address: ");
  Serial.println(adr);
  Serial.print("Writing elapsed time to EEPROM at address: ");
  Serial.println(tadr);

  writeFloatToEEPROM(adr, altRead(10));
  writeFloatToEEPROM(tadr, elapsed);
  apgDetect();

  // Debugging statements
  Serial.print("Reading elapsed time from EEPROM at address: ");
  Serial.println(tadr);
  Serial.print("Reading altitude from EEPROM at address: ");
  Serial.println(adr);

  Serial.print(readFloatFromEEPROM(tadr));
  Serial.print(", ");
  Serial.print(adr);
  Serial.print(", ");
  Serial.println(readFloatFromEEPROM(adr));

  adr += 8;
  delay(20);
  led(20, 1);
}
