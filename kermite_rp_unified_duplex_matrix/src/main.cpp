#include "Arduino.h"
#include "KeyScanner_DuplexMatrix.h"
#include "km1/KermiteCore.h"
#include "kpm/KeyScanner_DirectWired.h"
#include "kpm/KeyScanner_Dummy.h"
#include "kpm/KeyScanner_Encoders.h"
#include "kxe/BoardIndicator.h"
#include "kxe/OledDisplay.h"
#include "kxe/RgbLighting.h"

typedef struct {
  char marker[21];
  char keyboardName[17];
  uint8_t boardLedType;
  int8_t vlPinColumns[11];
  int8_t vlPinRows[11];
  int8_t vlPinsDirectWired[17];
  int8_t vlPinsEncoders[7];
  int8_t rgbLightingPin;
  uint8_t rgbLightingNumLeds;
  int8_t pinsOledI2C[2];
} FirmixParams;

volatile static const FirmixParams firmixParams = {
  .marker = "FIRMIX_STATIC_PARAMS",
  .keyboardName = { 0 },
  .boardLedType = 0,
  .vlPinColumns = { 0 },
  .vlPinRows = { 0 },
  .vlPinsDirectWired = { 0 },
  .vlPinsEncoders = { 0 },
  .rgbLightingPin = -1,
  .rgbLightingNumLeds = 0,
  .pinsOledI2C = { -1, -1 },
};

static KermiteCore kermite;
static IKeyScanner *keyMatrix;
static IKeyScanner *keyScannerDw;
static IKeyScanner *encodersScanner;
static BoardIndicator *boardIndicator;
static RgbLighting *rgbLighting;
static OledDisplay *oledDisplay;

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
        new KeyScanner_DuplexMatrix(pinColumns, pinRows, numColumns, numRows);
    keyMatrix->setKeyIndexBase(keyIndexBase);
    keyIndexBase += numColumns * numRows * 2;
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

  boardIndicator = new BoardIndicator(boardLedType);

  rgbLighting = new RgbLighting(firmixParams.rgbLightingPin,
                                firmixParams.rgbLightingNumLeds);
  oledDisplay =
      new OledDisplay(firmixParams.pinsOledI2C[0], firmixParams.pinsOledI2C[1]);
}

static void handleKeyStateChange(int keyIndex, bool pressed) {
  kermite.feedKeyState(keyIndex, pressed);
}

void setup() {
  rp2040.enableDoubleResetBootloader();
  setupModules();
  boardIndicator->initialize();
  keyMatrix->setKeyStateListener(handleKeyStateChange);
  keyMatrix->initialize();
  keyScannerDw->setKeyStateListener(handleKeyStateChange);
  keyScannerDw->initialize();
  encodersScanner->setKeyStateListener(handleKeyStateChange);
  encodersScanner->initialize();
  rgbLighting->initialize();
  oledDisplay->initialize();

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
  rgbLighting->update();
  oledDisplay->update();
  kermite.processUpdate();
  count++;
  delay(1);
}
