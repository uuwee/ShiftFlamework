#pragma once

#include <array>
#include <string>

namespace SF {

enum class ButtonState {
  DOWN,
  UP,
  HOLD,
  NEUTRAL,
};

enum class Keyboard : int {
  A = 65,
  B = 66,
  C = 67,
  D = 68,
  E = 69,
  F = 70,
  G = 71,
  H = 72,
  I = 73,
  J = 74,
  K = 75,
  L = 76,
  M = 77,
  N = 78,
  O = 79,
  P = 80,
  Q = 81,
  R = 82,
  S = 83,
  T = 84,
  U = 85,
  V = 86,
  W = 87,
  X = 88,
  Y = 89,
  Z = 90,
  ESCAPE = 27,
  SHIFT = 16,
  CONTROL = 17,
};

class Input {
 private:
  uint8_t key_state_before[256];
  std::array<ButtonState, 256> state;

 public:
 static std::string get_name() { return "Input"; }
  void initialize();

  void update();

  ButtonState get_keyboard_state(Keyboard button);
};
}  // namespace ShiftFlamework