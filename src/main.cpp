#include <Wire.h>
#include <Adafruit_BMP280.h>

#include <Arduino.h>
int i;
int d;
int dg;

int zero = 0;
int cur = 0;
int pre = 0;

int array[5];
int narray[5];

const int LED = 14;
const int but = 0;

const int apgthres = 200;

Adafruit_BMP280 bmp;
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getTemperatureSensor();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  while (!Serial) delay(100);
 
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);

  unsigned status;
  status = bmp.begin();
  if(!status){
    digitalWrite(LED, HIGH);
    while(1);
  }
  else{
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
  }

  
}
void led(int del, int amt){
  int j = 0;
  while(j != amt){
    digitalWrite(LED, HIGH);
    delay(del);
    digitalWrite(LED, LOW);
    delay(del);
    j += 1;
  }
}
void split(int num) {
  int va = num;
  int v = 0;
  i = 0;
  while(va != 0){va /= 10; dg += 1;}
  while(num != 0){
    array[i] = num % 10;
    num /= 10;
    i += 1;
  }
  d = dg;
  while(d != 0){
    //Serial.println(d);
    d -= 1;
    narray[v] = array[d];
  }

}
void pulse(){
  int l = 0;
  i = 0;
  while(i != dg){
    l = 0;
      if(narray[i] == 0){
        digitalWrite(LED, HIGH);
        delay(550);
        digitalWrite(LED, LOW);
        delay(200);
      }
      else{
        while(l != narray[i]){
          led(200, 1);
          l += 1;
        }
      }
      i += i;
      delay(500);
  }
  delay(2000);
}
void loop() {
  // put your main code here, to run repeatedly:

}

// put function definitions here:
