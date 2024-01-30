#pragma once

class KermiteBoardIndicator {
public:
  KermiteBoardIndicator(int boardLedType);
  void initialize();
  void update();
  void write(int index, bool value);
};
