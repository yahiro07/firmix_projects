#include "KeyScanner_DuplexMatrix.h"
#include <Arduino.h>

KeyScanner_DuplexMatrix::KeyScanner_DuplexMatrix(
    const uint8_t *_columnPins, const uint8_t *_rowPins, int _numColumns, int _numRows) {
  columnPins = _columnPins;
  rowPins = _rowPins;
  numColumns = _numColumns;
  numRows = _numRows;
  keyIndexBase = 0;
  keyStateListener = nullptr;
  inputKeyStates = new bool[numColumns * numRows * 2];
  keyStates = new bool[numColumns * numRows * 2];
}

void KeyScanner_DuplexMatrix::setKeyIndexBase(int _keyIndexBase) {
  keyIndexBase = _keyIndexBase;
}

void KeyScanner_DuplexMatrix::setKeyStateListener(KeyStateListenerFn fn) {
  keyStateListener = fn;
}

void KeyScanner_DuplexMatrix::initialize() {
  for (int i = 0; i < numColumns; i++) {
    pinMode(columnPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < numRows; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
}

void KeyScanner_DuplexMatrix::updateInput() {
  //scan col2row
  for (int row = 0; row < numRows; row++) {
    int rowPin = rowPins[row];
    pinMode(rowPin, OUTPUT);
    digitalWrite(rowPin, LOW);
    for (int col = 0; col < numColumns; col++) {
      int keyIndex = row * numColumns + col;
      inputKeyStates[keyIndex] = digitalRead(columnPins[col]) == LOW;
    }
    pinMode(rowPin, INPUT_PULLUP);
  }
  //scan row2col
  for (int col = 0; col < numColumns; col++) {
    int columnPin = columnPins[col];
    pinMode(columnPin, OUTPUT);
    digitalWrite(columnPin, LOW);
    for (int row = 0; row < numRows; row++) {
      int keyIndex = numColumns * numRows + row * numColumns + col;
      inputKeyStates[keyIndex] = digitalRead(rowPins[row]) == LOW;
    }
    pinMode(columnPin, INPUT_PULLUP);
  }

  int numKeySlots = numColumns * numRows * 2;
  for (int i = 0; i < numKeySlots; i++) {
    if (keyStateListener) {
      bool curr = keyStates[i];
      bool next = inputKeyStates[i];
      if (!curr && next) {
        keyStateListener(keyIndexBase + i, true);
      }
      if (curr && !next) {
        keyStateListener(keyIndexBase + i, false);
      }
    }
    keyStates[i] = inputKeyStates[i];
  }
}
