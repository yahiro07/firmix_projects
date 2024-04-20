#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <list>

// RP2040-GEEKのLCD表示参考
// https://tamanegi.digick.jp/computer-embedded/mcuboa/rp2040geek/

#define TFT_CS 9
#define TFT_RST 12
#define TFT_DC 8
#define TFT_MOSI 11
#define TFT_SCK 10

#define UART0_TX 28
#define UART0_RX 29

const int baudRates[] = {9600, 38400, 115200};

enum {
  Render_Full = 1,
  Render_Indicator = 2,
};

std::list<String> logLines;
int renderRequest = 0;
bool useLargeFont = true;
int baudIndex = 1;
int receiveIndicatorPhase = 0;
bool serialReceived = false;

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI1, TFT_CS, TFT_DC, TFT_RST);

bool readButton() { return BOOTSEL; }

void addLogLine(String text) {
  logLines.push_back(text);
  auto dispalyLinesCapacity = useLargeFont ? 6 : 12;
  while (logLines.size() > dispalyLinesCapacity) {
    logLines.pop_front();
  }
  renderRequest = Render_Full;
}

void updateSerialBaudRate() {
  auto baud = baudRates[baudIndex];
  Serial1.end();
  Serial1.begin(baud);
}

void setup(void) {
  SPI1.setSCK(TFT_SCK);
  SPI1.setTX(TFT_MOSI);
  tft.init(135, 240);
  tft.setRotation(3);
  tft.setTextSize(2);

  Serial1.setTX(UART0_TX);
  Serial1.setRX(UART0_RX);
  updateSerialBaudRate();
  renderRequest = Render_Full;
}

void taskReadSerial() {
  static String lineBuf = "";
  auto lineCapacity = useLargeFont ? 19 : 38;

  while (Serial1.available()) {
    serialReceived = true;
    char chr = Serial1.read();
    if (chr != '\n') {
      lineBuf += chr;
    }
    if (chr == '\n' || chr == '\0' || lineBuf.length() >= lineCapacity) {
      addLogLine(lineBuf);
      lineBuf = "";
    }
  }
}

void taskUpdateReceiveIndicator() {
  static uint tick = 0;
  if (++tick % 100 == 0) {
    if (serialReceived) {
      receiveIndicatorPhase = (receiveIndicatorPhase + 1) % 4;
      renderRequest = Render_Indicator;
      serialReceived = false;
    }
  }
}

void taskRenderDisplay() {
  if (renderRequest == Render_Full) {
    // 全体描画

    // 画面クリア
    tft.fillScreen(ST77XX_BLACK);

    // タイトル
    tft.setTextSize(useLargeFont ? 2 : 1);
    tft.setTextColor(ST77XX_BLUE);
    tft.setCursor(6, 2);
    tft.println("serial monitor");

    // ログ
    tft.setTextColor(ST77XX_GREEN);
    auto lineHeight = useLargeFont ? 20 : 10;
    auto numLogLines = logLines.size();
    auto i = 0;
    for (auto line : logLines) {
      tft.setCursor(6, lineHeight + lineHeight * i);
      tft.println(line);
      i++;
    }

    // 受信インジケータ
    auto px = 220 + receiveIndicatorPhase * 4;
    tft.fillRect(px, 4, 4, 4, ST77XX_YELLOW);

  } else if (renderRequest == Render_Indicator) {
    // 受信インジケータのみ更新
    for (auto i = 0; i < 4; i++) {
      auto px = 220 + i * 4;
      auto active = i == receiveIndicatorPhase;
      tft.fillRect(px, 4, 4, 4, active ? ST77XX_YELLOW : ST77XX_BLACK);
    }
  }
  renderRequest = 0;
}

void actionShiftBaudRate() {
  // ボタンを短く押したときのハンドラ
  // BuadRateを 9600-->38400-->12500の順にシフト
  // デフォルトは38400bps
  baudIndex = (baudIndex + 1) % 3;
  auto baud = baudRates[baudIndex];
  String message = "baud_rate:" + String(baud);
  addLogLine(message);
  updateSerialBaudRate();
}

void actionShiftFontSize() {
  // ボタンを長押ししたときのハンドラ
  // 文字サイズの大小切り替え
  // デフォルトは大
  useLargeFont = !useLargeFont;
  String message = "font_size:" + String(useLargeFont ? "large" : "small");
  addLogLine(message);
}

void taskHandleButton() {
  static int tick = 0;
  static int prevPressed = false;
  static int holdTick = 0;
  static bool actionResolved = false;
  if (tick % 10 == 0) {
    bool pressed = readButton();
    if (!prevPressed && pressed) {
      // ボタン押し始め
      holdTick = 0;
      actionResolved = false;
    }
    if (pressed && !actionResolved) {
      holdTick++;
      if (holdTick >= 50) {
        // ボタン押してを500ms以上ホールド
        actionShiftFontSize();
        actionResolved = true;
      }
    }
    if (prevPressed && !pressed && !actionResolved) {
      // ボタンを押してすぐ離した場合
      actionShiftBaudRate();
      actionResolved = true;
    }
    prevPressed = pressed;
  }
  tick++;
}

void loop() {
  taskReadSerial();
  taskUpdateReceiveIndicator();
  taskRenderDisplay();
  taskHandleButton();
  delay(1);
}