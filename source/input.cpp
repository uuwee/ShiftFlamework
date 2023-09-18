#include "input.hpp"

#include <iostream>
using namespace ShiftFlamework;

#if defined(__EMSCRIPTEN__)
static Keyboard input_to_code(const char input[32]) {
  if (*input == 'A' || *input == 'a') {
    return Keyboard::A;
  }
  return Keyboard::ESCAPE;
}
static EM_BOOL keyboard_callback(int event_type,
                                 const EmscriptenKeyboardEvent* event,
                                 void* userdata) {
  auto input = reinterpret_cast<Input*>(userdata);
  auto code = input_to_code(event->key);

  switch (event_type) {
    case EMSCRIPTEN_EVENT_KEYDOWN:
      if (input->state.at(static_cast<int>(code)) == ButtonState::NEUTRAL)
        input->state.at(static_cast<int>(code)) = ButtonState::DOWN;
      else if (input->state.at(static_cast<int>(code)) == ButtonState::DOWN)
        input->state.at(static_cast<int>(code)) = ButtonState::HOLD;
      break;
    case EMSCRIPTEN_EVENT_KEYUP:
      input->state.at(static_cast<int>(input_to_code(event->key))) =
          ButtonState::UP;
      break;
  }

  if (input->state.at(static_cast<int>(Keyboard::A)) == ButtonState::DOWN)
    std::cout << "a down!" << std::endl;
  return true;
}
#endif

void Input::initialize() {
#if defined(__EMSCRIPTEN__)
  emscripten_set_keydown_callback("canvas", this, true, keyboard_callback);
  emscripten_set_keyup_callback("canvas", this, true, keyboard_callback);
#endif
}

void Input::update() {
#if defined(_MSC_VER)
  BYTE key_state_current[256];
  GetKeyboardState(key_state_current);
  for (int i = 0; i < 256; i++) {
    if ((key_state_current[i] & 0x80) && (key_state_before[i] & 0x80)) {
      state.at(i) = ButtonState::HOLD;
    } else if ((key_state_current[i] & 0x80) && !(key_state_before[i] & 0x80)) {
      state.at(i) = ButtonState::DOWN;
    } else if (!(key_state_current[i] & 0x80) && (key_state_before[i] & 0x80)) {
      state.at(i) = ButtonState::UP;
    } else {
      state.at(i) = ButtonState::NEUTRAL;
    }

    key_state_before[i] = key_state_current[i];
  }
#elif defined(__EMSCRIPTEN__)
  for (int i = 0; i < 256; i++) {
    if (i == 65) {
      if (state.at(65) == ButtonState::DOWN) {
        // std::cout << "DOWN" << std::endl;
      } else if (state.at(65) == ButtonState::HOLD) {
        // std::cout << "HOLD" << std::endl;
      }
    }
    if (state.at(i) == ButtonState::DOWN) {
      state.at(i) = ButtonState::HOLD;
    } else if (state.at(i) == ButtonState::UP) {
      state.at(i) = ButtonState::NEUTRAL;
    }
  }
#endif
}