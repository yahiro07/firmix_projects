#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

const int pin_neopixel = 12;
const int pin_neopixel_power = 11;

static Adafruit_NeoPixel neoPixel(1, pin_neopixel, NEO_GRB);

void setup() {
  pinMode(pin_neopixel_power, OUTPUT);
  digitalWrite(pin_neopixel_power, HIGH);
  pinMode(pin_neopixel, OUTPUT);
  neoPixel.setBrightness(64);
  neoPixel.begin();
}

static void putLedColor(uint32_t color) {
  neoPixel.clear();
  neoPixel.setPixelColor(0, color);
  neoPixel.show();
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  if (cc == 0) {
    putLedColor(0xFF0000);
  } else if (cc == 250) {
    putLedColor(0x00FF00);
  } else if (cc == 500) {
    putLedColor(0x0000FF);
  } else if (cc == 750) {
    putLedColor(0);
  }
  count++;
  delay(10); // debug
}
