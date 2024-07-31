#pragma once

// libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace heh {

class Keyboard {
 public:
  enum class Key {
    kLeftAlt = GLFW_KEY_LEFT_ALT,
    kLeftControl = GLFW_KEY_LEFT_CONTROL,
    kLeftShift = GLFW_KEY_LEFT_SHIFT,
    kSpace = GLFW_KEY_SPACE,
    kApostrophe = GLFW_KEY_APOSTROPHE,
    kComma = GLFW_KEY_COMMA,
    kMinus = GLFW_KEY_MINUS,
    kPeriod = GLFW_KEY_PERIOD,
    kSlash = GLFW_KEY_SLASH,
    k0 = GLFW_KEY_0,
    k1 = GLFW_KEY_1,
    k2 = GLFW_KEY_2,
    k3 = GLFW_KEY_3,
    k4 = GLFW_KEY_4,
    k5 = GLFW_KEY_5,
    k6 = GLFW_KEY_6,
    k7 = GLFW_KEY_7,
    k8 = GLFW_KEY_8,
    k9 = GLFW_KEY_9,
    kSemicolon = GLFW_KEY_SEMICOLON,
    kEqual = GLFW_KEY_EQUAL,
    kA = GLFW_KEY_A,
    kB = GLFW_KEY_B,
    kC = GLFW_KEY_C,
    kD = GLFW_KEY_D,
    kE = GLFW_KEY_E,
    kF = GLFW_KEY_F,
    kG = GLFW_KEY_G,
    kH = GLFW_KEY_H,
    kI = GLFW_KEY_I,
    kJ = GLFW_KEY_J,
    kK = GLFW_KEY_K,
    kL = GLFW_KEY_L,
    kM = GLFW_KEY_M,
    kN = GLFW_KEY_N,
    kO = GLFW_KEY_O,
    kP = GLFW_KEY_P,
    kQ = GLFW_KEY_Q,
    kR = GLFW_KEY_R,
    kS = GLFW_KEY_S,
    kT = GLFW_KEY_T,
    kU = GLFW_KEY_U,
    kV = GLFW_KEY_V,
    kW = GLFW_KEY_W,
    kX = GLFW_KEY_X,
    kY = GLFW_KEY_Y,
    kZ = GLFW_KEY_Z,
    kLeftBracket = GLFW_KEY_LEFT_BRACKET,
    kBackslash = GLFW_KEY_BACKSLASH,
    kRightBracket = GLFW_KEY_RIGHT_BRACKET,
    kGraveAccent = GLFW_KEY_GRAVE_ACCENT,
    kEscape = GLFW_KEY_ESCAPE,
    kEnter = GLFW_KEY_ENTER,
    kTab = GLFW_KEY_TAB,
    kBackspace = GLFW_KEY_BACKSPACE,
    kInsert = GLFW_KEY_INSERT,
    kDelete = GLFW_KEY_DELETE,

    kRight = GLFW_KEY_RIGHT,
    kLeft = GLFW_KEY_LEFT,
    kDown = GLFW_KEY_DOWN,
    kUp = GLFW_KEY_UP,

    kF1 = GLFW_KEY_F1,
    kF2 = GLFW_KEY_F2,
    kF3 = GLFW_KEY_F3,
    kF4 = GLFW_KEY_F4,
    kF5 = GLFW_KEY_F5,
    kF6 = GLFW_KEY_F6,
    kF7 = GLFW_KEY_F7,
    kF8 = GLFW_KEY_F8,
    kF9 = GLFW_KEY_F9,
    kF10 = GLFW_KEY_F10,
    kF11 = GLFW_KEY_F11,
    kF12 = GLFW_KEY_F12,
  };  // enum class Key

  static void HandleKey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      keys_[key] = true;
      keys_held_[key] = true;
      keys_released_[key] = false;
    } else if (action == GLFW_RELEASE) {
      keys_[key] = false;
      keys_held_[key] = false;
      keys_released_[key] = true;
    }
  }
  static bool IsKeyPressed(Key key) { return keys_[static_cast<int>(key)]; }
  static bool IsKeyReleased(Key key) { return keys_released_[static_cast<int>(key)]; }
  static bool IsKeyHeld(Key key) { return keys_held_[static_cast<int>(key)]; }

 private:
  static bool keys_[GLFW_KEY_LAST + 1];
  static bool keys_released_[GLFW_KEY_LAST + 1];
  static bool keys_held_[GLFW_KEY_LAST + 1];
};

class Mouse {
 public:
  enum class Button {
    kLeft = GLFW_MOUSE_BUTTON_LEFT,
    kRight = GLFW_MOUSE_BUTTON_RIGHT,
    kMiddle = GLFW_MOUSE_BUTTON_MIDDLE,
  };  // enum class Button

  static void HandleMouseButton(int button, int action, int mods);
  static void HandleCursorPosition(double xpos, double ypos);
  static void HandleScroll(double xoffset, double yoffset);
  static bool IsButtonPressed(int button) { return buttons_[button]; }
  static bool IsButtonReleased(int button) { return buttons_released_[button]; }
  static bool IsButtonHeld(int button) { return buttons_held_[button]; }
  static double GetX() { return x_; }
  static double GetY() { return y_; }
  static double GetDX() { return dx_; }
  static double GetDY() { return dy_; }
  static double GetScrollX() { return scroll_x_; }
  static double GetScrollY() { return scroll_y_; }

  inline static void ResetDeltas() {
    dx_ = 0.0;
    dy_ = 0.0;
  }

 private:
  static bool buttons_[GLFW_MOUSE_BUTTON_LAST + 1];
  static bool buttons_released_[GLFW_MOUSE_BUTTON_LAST + 1];
  static bool buttons_held_[GLFW_MOUSE_BUTTON_LAST + 1];
  static double x_;
  static double y_;
  static double dx_;
  static double dy_;
  static double scroll_x_;
  static double scroll_y_;
};

}  // namespace heh