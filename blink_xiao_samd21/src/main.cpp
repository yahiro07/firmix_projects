#include <Arduino.h>

const int pin_led = 13;

void setup() { pinMode(pin_led, OUTPUT); }

void loop() {
  static int count = 0;
  int cc = count % 1000;
  digitalWrite(pin_led, !(cc < 100));
  count++;
  delay(2);
}
