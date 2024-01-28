#include <Arduino.h>
#include <KermiteCore.h>
#include <kpm/BoardLED.h>
#include <kpm/BoardLED_Dummy.h>
#include <kpm/BoardLED_NeoPixel.h>
#include <kpm/KeyMatrix.h>

typedef struct {
  char marker[21];
  char keyboardName[17];
  uint8_t boardLedType;
  uint8_t vlPinColumns[17];
  uint8_t vlPinRows[17];
} FirmixParams;

volatile static const FirmixParams firmixParams = {
    .marker = "FIRMIX_STATIC_PARAMS",
    .keyboardName = {0},
    .boardLedType = 0,
    .vlPinColumns = {0},
    .vlPinRows = {0},
};

static KermiteCore kermite;
static KeyMatrix *keyMatrix = nullptr;
static IBoardLED *boardLED = nullptr;

static void setupModules() {
  int boardLedType = firmixParams.boardLedType;
  int numColumns = firmixParams.vlPinColumns[0];
  int numRows = firmixParams.vlPinRows[0];

  if (numColumns > 0 && numRows > 0) {
    uint8_t *pinColumns = (uint8_t *)(&firmixParams.vlPinColumns[1]);
    uint8_t *pinRows = (uint8_t *)(&firmixParams.vlPinRows[1]);
    keyMatrix = new KeyMatrix(pinColumns, pinRows, numColumns, numRows);
  }
  if (boardLedType == 1) {
    boardLED = new BoardLED(25, 25); // pico
  } else if (boardLedType == 2) {
    boardLED = new BoardLED_NeoPixel(17, 0x40); // kb2040
  } else if (boardLedType == 3) {
    boardLED = new BoardLED_NeoPixel(12, 0x40, 11); // xiao rp2040
  } else if (boardLedType == 4) {
    boardLED = new BoardLED_NeoPixel(16, 0x40); // rp2040-zero
  } else if (boardLedType == 5) {
    boardLED = new BoardLED(18, 19, 20, true); // tiny2040
  } else if (boardLedType == 6) {
    boardLED = new BoardLED_NeoPixel(25, 0x40); // promicro rp2040
  } else {
    boardLED = new BoardLED_Dummy();
  }
}

static int pressedKeyCount = 0;

static void handleKeyStateChange(int keyIndex, bool pressed) {
  kermite.feedKeyState(keyIndex, pressed);
  pressedKeyCount += (pressed ? 1 : -1);
  boardLED->write(1, pressedKeyCount > 0);
}

void setup() {
  setupModules();
  boardLED->initialize();

  if (keyMatrix) {
    keyMatrix->setKeyStateListener(handleKeyStateChange);
    keyMatrix->initialize();
  }

  if (firmixParams.keyboardName[0] != '\0') {
    kermite.setKeyboardName((const char *)firmixParams.keyboardName);
  } else {
    kermite.setKeyboardName("mykeeb");
  }

  kermite.begin();
}

void loop() {
  static int count = 0;
  boardLED->write(0, count % 1000 == 0);
  if (count % 10 == 0) {
    if (keyMatrix) {
      keyMatrix->updateInput();
    }
  }
  kermite.processUpdate();
  count++;
  delay(1);
}
