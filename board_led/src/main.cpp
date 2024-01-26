#include <Arduino.h>
#include <kpm/BoardLED.h>
#include <kpm/BoardLED_Dummy.h>
#include <kpm/BoardLED_NeoPixel.h>

typedef struct {
  const char marker[8];
  uint8_t board_led_type;
} Stencil;

volatile static const Stencil stencil = {
    .marker = "PINDEFS",
    .board_led_type = 0,
};

static IBoardLED *boardLED;

void setup() {
  rp2040.enableDoubleResetBootloader();
  int boardLedType = stencil.board_led_type;
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
  boardLED->initialize();
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  boardLED->write(0, cc < 250);
  boardLED->write(1, 250 <= cc && cc < 500);
  boardLED->write(2, 500 <= cc && cc < 750);
  count++;
  delay(1);
}
