#include "KermiteBoardIndicator.h"
#include "Arduino.h"
#include "km1/domain/commandDefinitions.h"
#include "km1/domain/configManager.h"
#include "km1/domain/keyboardMain.h"
#include "kpm/BoardLED.h"
#include "kpm/BoardLED_Dummy.h"
#include "kpm/BoardLED_NeoPixel.h"

static IBoardLED *boardLED;

/*
内部状態を6個保持しておき、これをRGB3個のLEDに出力する
0/1/2: ユーザアプリケーションによるデバッグ目的などで利用する想定
3/4/5: フレームワークによる規定の状態出力実装で使用
states[0] || states[3] --> R
states[1] || states[4] --> G
states[2] || states[5] --> B
*/
static bool states[6] = {false};

static int pressedKeyCount = 0;

// 動的に変更可能なオプション
static bool optionAffectKeyHoldStateToLed = true;
static bool optionUseHeartbeatLed = true;

static void boardIndicator_initializeBoardLedInstance(int boardLedType) {
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

static void boardIndicator_write(int inputIndex, bool value) {
  states[inputIndex] = value;
  int index = inputIndex % 3;
  boardLED->write(index, states[index] || states[index + 3]);
}

static void handleKeySlotStateChange(uint8_t slotIndex, bool isDown) {
  pressedKeyCount += isDown ? 1 : -1;
  if (optionAffectKeyHoldStateToLed) {
    boardIndicator_write(5, pressedKeyCount > 0);
  }
}

static void handleParameterValueChange(uint8_t eventType, uint8_t slotIndex,
                                       uint8_t value) {
  if (eventType == ParameterChangeEventType_ChangedAll) {
    optionAffectKeyHoldStateToLed =
        configManager_readParameter(SystemParameter_KeyHoldIndicatorLed);
    optionUseHeartbeatLed =
        configManager_readParameter(SystemParameter_HeartbeatLed);
  } else if (eventType == ParameterChangeEventType_ChangedSingle) {
    if (slotIndex == SystemParameter_KeyHoldIndicatorLed) {
      optionAffectKeyHoldStateToLed = !!value;
    } else if (slotIndex == SystemParameter_HeartbeatLed) {
      optionUseHeartbeatLed = !!value;
    }
  }
}

static void boardIndicator_initialize() {
  configManager_setParameterExposeFlag(SystemParameter_HeartbeatLed);
  configManager_setParameterExposeFlag(SystemParameter_KeyHoldIndicatorLed);
  configManager_addParameterChangeListener(handleParameterValueChange);
  keyboardMain_setKeySlotStateChangedCallback(handleKeySlotStateChange);
  boardLED->initialize();
}

static void boardIndicator_update() {
  static uint32_t taskLedNextTimeMs = 0;
  uint32_t timeMs = millis();
  if (timeMs > taskLedNextTimeMs) {
    if (optionUseHeartbeatLed) {
      boardIndicator_write(4, true);
      delay(2);
      boardIndicator_write(4, false);
    }
    taskLedNextTimeMs = timeMs + 3000;
  }
}

KermiteBoardIndicator::KermiteBoardIndicator(int boardLedType) {
  boardIndicator_initializeBoardLedInstance(boardLedType);
}

void KermiteBoardIndicator::initialize() { boardIndicator_initialize(); }

void KermiteBoardIndicator::update() { boardIndicator_update(); }

void KermiteBoardIndicator::write(int index, bool value) {
  boardIndicator_write(index, value);
}
