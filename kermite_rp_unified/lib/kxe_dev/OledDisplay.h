#pragma once
#include <stdint.h>

class OledDisplay {
public:
  OledDisplay(int pinSDA, int pinSCL);
  void setCustomLogo(const uint8_t *logoData);
  void initialize();
  void update();
};
