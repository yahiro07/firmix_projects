#include "BoardLED_Kermite.h"
#include <Arduino.h>
#include <kpm/BoardLED.h>
#include <kpm/BoardLED_Dummy.h>
#include <kpm/BoardLED_NeoPixel.h>

BoardLED_Kermite::BoardLED_Kermite(int boardLedType) {
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

void BoardLED_Kermite::initialize(KermiteCore &kermite) {
  boardLED->initialize();
}

void BoardLED_Kermite::write(int index, bool value) {
  boardLED->write(index, value);
}

void BoardLED_Kermite::toggle(int index) { boardLED->toggle(index); }
