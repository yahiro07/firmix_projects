#include <Arduino.h>

const int pin_led = 13;

void setup() {
  Serial.begin(9600); // required for auto-resetting when uploading a new binary
  pinMode(pin_led, OUTPUT);
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  digitalWrite(pin_led, !(cc < 100));
  count++;
  delay(1);
}
