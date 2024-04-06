#include "Arduino.h"
#include "kermite_fullkit_resolve_ldf.h"
#include "km1/KermiteCore.h"
#include "kpm/BoardLED_NeoPixel.h"
#include "kpm/KeyScanner_KeyMatrix.h"

BoardLED_NeoPixel boardLED(25, 0x40);

KermiteCore kermite;

const int numColumns = 6;
const int numRows = 8;
const int pinColumns[numColumns] = {5, 4, 26, 27, 28, 29};
const int pinRows[numRows] = {
    22, 20, 23, 21, 6, 7, 8, 9,
};

KeyScanner_KeyMatrix keyMatrix(pinColumns, pinRows, numColumns, numRows);

int pressedKeyCount = 0;
bool heartbeatFlash = false;

void updateLedOutput() {
  boardLED.write(1, heartbeatFlash);
  boardLED.write(2, pressedKeyCount > 0);
}

void handleKeyStateChange(int keyIndex, bool pressed) {
  kermite.feedKeyState(keyIndex, pressed);
  pressedKeyCount += (pressed ? 1 : -1);
}

void setup() {
  rp2040.enableDoubleResetBootloader();
  boardLED.initialize();

  keyMatrix.setKeyStateListener(handleKeyStateChange);
  keyMatrix.initialize();

  kermite.setKeyboardName("Astelia");
  kermite.setProductionMode();
  kermite.begin();
}

void loop() {
  static int count = 0;

  heartbeatFlash = count % 1000 == 0;
  if (count % 10 == 0) {
    keyMatrix.updateInput();
  }
  updateLedOutput();
  kermite.processUpdate();
  count++;
  delay(1);
}
