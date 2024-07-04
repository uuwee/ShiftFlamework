#include "input.hpp"

#include <iostream>
using namespace SF;

#if defined(_MSC_VER)
#include <windows.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#if defined(__EMSCRIPTEN__)
static Keyboard input_to_code(const char input[32]) {
  if ((*input) - 'A' >= 0 && (*input) - 'A' < 26) {
    return static_cast<Keyboard>(*input - 'A' + 65);
  } else if ((*input) - 'a' >= 0 && (*input) - 'a' < 26) {
    return static_cast<Keyboard>(*input - 'a' + 65);
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
  return true;
}
#endif

void Input::initialize() {
  for (int i = 0; i < state.size(); i++) {
    state.at(i) = ButtonState::NEUTRAL;
  }
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
    if (state.at(i) == ButtonState::DOWN) {
      state.at(i) = ButtonState::HOLD;
    } else if (state.at(i) == ButtonState::UP) {
      state.at(i) = ButtonState::NEUTRAL;
    }
  }
#endif
}

ButtonState Input::get_keyboard_state(Keyboard button) {
  return state.at(static_cast<int>(button));
}