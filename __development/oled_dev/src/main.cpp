
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Arduino.h"
#include "kpm/BoardLED_NeoPixel.h"

static IBoardLED *boardLED = new BoardLED_NeoPixel(16, 0x40); // rp2040-zero

static bool displayInitialized = false;
static Adafruit_SSD1306 display(128, 32, &Wire, -1);

void display_init() {
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  displayInitialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (displayInitialized) {
    display.setTextColor(SSD1306_WHITE);
  }
}

void display_draw() {
  if (!displayInitialized) {
    return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(4, 0);
  display.println("hello world");
  display.setCursor(4, 10);
  display.println("aaa bbb ccc");
  display.display();
}

void setup() {
  boardLED->initialize();
  display_init();
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  boardLED->write(0, cc <= 10);
  count++;
  delay(1);
  if (count % 100 == 0) {
    display_draw();
  }
}
