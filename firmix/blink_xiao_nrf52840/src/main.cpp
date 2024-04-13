#include <Arduino.h>

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  digitalWrite(LED_RED, !(cc < 250));
  digitalWrite(LED_GREEN, !(250 <= cc && cc < 500));
  digitalWrite(LED_BLUE, !(500 <= cc && cc < 750));
  count++;
  delay(1);
}
