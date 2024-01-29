#pragma once
#include <KermiteCore.h>
#include <kpm/IBoardLED.h>

class KermiteBoardIndicator {
public:
  KermiteBoardIndicator(int boardLedType);
  void initialize();
  void update();
  void write(int index, bool value);
};
