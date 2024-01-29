#include <Arduino.h>
#include <KermiteCore.h>
#include <kpm/KeyScanner_DirectWired.h>
#include <kpm/KeyScanner_Dummy.h>
#include <kpm/KeyScanner_Encoders.h>
#include <kpm/KeyScanner_KeyMatrix.h>
#include <kxl/KermiteBoardIndicator.h>

typedef struct {
  char marker[21];
  char keyboardName[17];
  uint8_t boardLedType;
  uint8_t vlPinColumns[17];
  uint8_t vlPinRows[17];
  uint8_t vlPinsDirectWired[17];
  uint8_t vlPinsEncoders[7];
} FirmixParams;

volatile static const FirmixParams firmixParams = {
    .marker = "FIRMIX_STATIC_PARAMS",
    .keyboardName = {0},
    .boardLedType = 0,
    .vlPinColumns = {0},
    .vlPinRows = {0},
    .vlPinsDirectWired = {0},
    .vlPinsEncoders = {0},
};

static KermiteCore kermite;
static IKeyScanner *keyMatrix;
static IKeyScanner *keyScannerDw;
static IKeyScanner *encodersScanner;
static KermiteBoardIndicator *boardIndicator;

static void setupModules() {
  int boardLedType = firmixParams.boardLedType;
  int numColumns = firmixParams.vlPinColumns[0];
  int numRows = firmixParams.vlPinRows[0];
  int numDirectWiredPins = firmixParams.vlPinsDirectWired[0];
  int numEncoderPins = firmixParams.vlPinsEncoders[0];

  int keyIndexBase = 0;
  if (numColumns > 0 && numRows > 0) {
    auto *pinColumns = (const uint8_t *)firmixParams.vlPinColumns + 1;
    auto *pinRows = (const uint8_t *)firmixParams.vlPinRows + 1;
    keyMatrix =
        new KeyScanner_KeyMatrix(pinColumns, pinRows, numColumns, numRows);
    keyMatrix->setKeyIndexBase(keyIndexBase);
    keyIndexBase += numColumns * numRows;
  } else {
    keyMatrix = new KeyScanner_Dummy();
  }
  if (numDirectWiredPins) {
    auto pins = (const uint8_t *)firmixParams.vlPinsDirectWired + 1;
    keyScannerDw = new KeyScanner_DirectWired(numDirectWiredPins, pins);
    keyScannerDw->setKeyIndexBase(keyIndexBase);
    keyIndexBase += numDirectWiredPins;
  } else {
    keyScannerDw = new KeyScanner_Dummy();
  }

  if (numEncoderPins) {
    auto pins = (const uint8_t *)firmixParams.vlPinsEncoders + 1;
    encodersScanner = new KeyScanner_Encoders(numEncoderPins, pins);
    encodersScanner->setKeyIndexBase(keyIndexBase);
    keyIndexBase += numEncoderPins;
  } else {
    encodersScanner = new KeyScanner_Dummy();
  }

  boardIndicator = new KermiteBoardIndicator(boardLedType);
}

static int pressedKeyCount = 0;

static void handleKeyStateChange(int keyIndex, bool pressed) {
  kermite.feedKeyState(keyIndex, pressed);
}

void setup() {
  setupModules();
  boardIndicator->initialize();
  keyMatrix->setKeyStateListener(handleKeyStateChange);
  keyMatrix->initialize();
  keyScannerDw->setKeyStateListener(handleKeyStateChange);
  keyScannerDw->initialize();
  encodersScanner->setKeyStateListener(handleKeyStateChange);
  encodersScanner->initialize();

  if (firmixParams.keyboardName[0] != '\0') {
    kermite.setKeyboardName((const char *)firmixParams.keyboardName);
  } else {
    kermite.setKeyboardName("mykeeb");
  }

  kermite.setProductionMode();
  kermite.begin();
}

void loop() {
  static int count = 0;
  if (count % 10 == 0) {
    keyMatrix->updateInput();
    keyScannerDw->updateInput();
    encodersScanner->updateInput();
  }
  boardIndicator->update();
  kermite.processUpdate();
  count++;
  delay(1);
}
