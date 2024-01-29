
#include <Arduino.h>
#include <kpm/BoardLED_NeoPixel.h>
#include <kpm/KeyScanner_Encoders.h>

static IBoardLED *boardLED = new BoardLED_NeoPixel(16, 0x40); // rp2040-zero

const uint8_t encoderPins[] = {2, 3, 4, 5};
static KeyScanner_Encoders *encodersScanner =
    new KeyScanner_Encoders(4, encoderPins);

static int pressedKeyCount = 0;

static void handleKeyStateChange(int keyIndex, bool pressed) {
  // kermite.feedKeyState(keyIndex, pressed);
  pressedKeyCount += (pressed ? 1 : -1);
  boardLED->write(1, pressedKeyCount > 0);
}

void setup() {
  Serial1.begin(38400);
  boardLED->initialize();
  encodersScanner->setKeyIndexBase(0);
  encodersScanner->setKeyStateListener(handleKeyStateChange);
  encodersScanner->initialize();
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  boardLED->write(0, cc <= 10);
  count++;
  encodersScanner->updateInput();
  delay(1);
}
