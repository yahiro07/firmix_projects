#include <Arduino.h>
#include <kpm/BoardLED_NeoPixel.h>
#include <pio_encoder.h>

typedef void (*KeyStateListenerFn)(int keyIndex, bool pressed);

class EncoderWrapper {
private:
  PioEncoder *pioEncoder = nullptr;
  int keyIndexBase = 0;
  KeyStateListenerFn keyStateListener = nullptr;
  int count = 0;
  int outputHoldKeyIndex = 0;

public:
  EncoderWrapper(int pin) {
    // pio1を明示的に指定しないと、BoardLEDが利用するAdafruit_NeoPixelの実装とバッティングしてうまく動かない(?)
    pioEncoder = new PioEncoder(pin, pio1);
  }
  void setKeyIndexBase(int _keyIndexBase) { keyIndexBase = _keyIndexBase; }
  void setKeyStateListener(KeyStateListenerFn fn) { keyStateListener = fn; }
  void initialize() { pioEncoder->begin(); }
  void updateInput() {
    if (!keyStateListener)
      return;
    if (outputHoldKeyIndex) {
      // キー状態出力中ならそれを解除
      keyStateListener(outputHoldKeyIndex, false);
      outputHoldKeyIndex = 0;
    } else {
      // カウントを読み取ってキー状態を出力
      int nextCount = pioEncoder->getCount();
      int dir = nextCount - count;
      if (dir != 0) {
        int keyIndex = (dir > 0) ? keyIndexBase : (keyIndexBase + 1);
        keyStateListener(keyIndex, true);
        outputHoldKeyIndex = keyIndex;
      }
      count = nextCount;
    }
  }
};

static IBoardLED *boardLED = new BoardLED_NeoPixel(16, 0x40); // rp2040-zero

static EncoderWrapper encoder1(2); // gp2, gp3
static EncoderWrapper encoder2(4); // gp4, gp5

static int pressedKeyCount = 0;

static void handleKeyStateChange(int keyIndex, bool pressed) {
  // kermite.feedKeyState(keyIndex, pressed);
  pressedKeyCount += (pressed ? 1 : -1);
  boardLED->write(1, pressedKeyCount > 0);
}

void setup() {
  Serial1.begin(38400);
  boardLED->initialize();
  encoder1.setKeyIndexBase(0);
  encoder1.setKeyStateListener(handleKeyStateChange);
  encoder1.initialize();
  encoder2.setKeyIndexBase(2);
  encoder2.setKeyStateListener(handleKeyStateChange);
  encoder2.initialize();
}

void loop() {
  static int count = 0;
  int cc = count % 1000;
  boardLED->write(0, cc <= 10);
  count++;

  // updateInput()を呼ぶとLEDの点滅周期が延びる
  // 内部で呼んでいるpioEncoder->update()で無駄にステップ数を消費している(?)
  // 要調査
  encoder1.updateInput();
  encoder2.updateInput();

  delay(1);
}
