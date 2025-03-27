#ifndef BLINK_HPP
#define BLINK_HPP

#include <Arduino.h>

int array[5];
int narray[5];

int dg;

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
  
  // Have proper comment blocks above each function describing the inputs and outputs
  // This makes your code easier to understand and maintain
  int split(int num) {
    num = (int)num;

    int va = num;
    int v = 0;
    int i = 0;
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
    int d = dg;
    // while equivalence checking is super dangerous inn your code
    // use < or > instead and for loops are generally better than while loops
    // in this particular condition
    while (d != 0) {
      d -= 1;
      narray[v] = array[d];
      v += 1; // Increment v to fill narray correctly
    }
    return num;
  
  }
  
  void pulse() {
    int l = 0;
    int i = 0;
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

#endif