#pragma once
#include <KermiteCore.h>
#include <kpm/IBoardLED.h>

class BoardLED_Kermite {
private:
  IBoardLED *boardLED;

public:
  BoardLED_Kermite(int boardLedType);
  void initialize(KermiteCore &kermite);
  void write(int index, bool value);
  void toggle(int index);
};
