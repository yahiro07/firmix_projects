#include "Arduino.h"
#include "kermite_fullkit_resolve_ldf.h"
#include "km1/KermiteCore.h"
#include "kpm/BoardLED.h"
#include "kpm/KeyScanner_KeyMatrix.h"

BoardLED boardLED(25);

KermiteCore kermite;

const int numColumns = 6;
const int numRows = 5;
const int pinColumns[numColumns] = {2, 3, 4, 5, 6, 7};
const int pinRows[numRows] = {8, 9, 10, 11, 12};

KeyScanner_KeyMatrix keyMatrix(pinColumns, pinRows, numColumns, numRows);

int pressedKeyCount = 0;
bool heartbeatFlash = false;

void updateLedOutput() {
  boardLED.write(0, (pressedKeyCount > 0) || heartbeatFlash);
}

void handleKeyStateChange(int keyIndex, bool pressed) {
  kermite.feedKeyState(keyIndex, pressed);
  pressedKeyCount += (pressed ? 1 : -1);
}

void setup() {
  boardLED.initialize();

  keyMatrix.setKeyStateListener(handleKeyStateChange);
  keyMatrix.initialize();

  kermite.setKeyboardName("pipi gherkin");
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
