#pragma once
#include "kpm/IKeyScanner.h"
#include <stdint.h>

class KeyScanner_DuplexMatrix : public IKeyScanner {
private:
  const uint8_t *columnPins;
  const uint8_t *rowPins;
  int numColumns;
  int numRows;
  int keyIndexBase;
  KeyStateListenerFn keyStateListener;
  bool *inputKeyStates;

public:
  bool *keyStates;

  KeyScanner_DuplexMatrix(
      const uint8_t *_columnPins, const uint8_t *_rowPins, int _numColumns, int _numRows);

  void setKeyIndexBase(int _keyIndexBase);
  void setKeyStateListener(KeyStateListenerFn fn);
  void initialize();
  void updateInput();
};
