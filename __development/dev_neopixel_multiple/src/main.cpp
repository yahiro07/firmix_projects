
#include <Arduino.h>
#include <kpm/BoardLED_NeoPixel.h>
#include <kpm/KeyScanner_Encoders.h>

static IBoardLED *boardLED = new BoardLED_NeoPixel(16, 0x40); // rp2040-zero

static Adafruit_NeoPixel pixels(4, 14);

void setup() {
  boardLED->initialize();
  pixels.begin();
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  boardLED->write(0, cc <= 10);
  count++;
  delay(1);
  if (count % 100 == 0) {
    uint8_t rr = count / 10;
    for (int i = 0; i < 4; i++) {
      pixels.setPixelColor(i, pixels.Color(rr, 0, 0));
    }
    pixels.show();
  }
}
