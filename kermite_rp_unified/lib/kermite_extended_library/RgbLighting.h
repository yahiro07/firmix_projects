#pragma once

class RgbLighting {
public:
  RgbLighting(int pin, int numLeds);
  void initialize();
  void update();
};
