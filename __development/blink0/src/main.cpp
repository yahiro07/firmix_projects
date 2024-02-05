#include <Arduino.h>

static const int pinLed0 = 2;
static const int pinLed1 = 4;
static const int pinButton = 8;

void setup() {
  rp2040.enableDoubleResetBootloader();
  pinMode(pinLed0, OUTPUT);
  pinMode(pinLed1, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
}

void loop() {
  static int count = 0;
  if (count % 1000 == 0) {
    digitalWrite(pinLed0, !digitalRead(pinLed0));
  }
  digitalWrite(pinLed1, !digitalRead(pinButton));
  count++;
  delay(1);
}
