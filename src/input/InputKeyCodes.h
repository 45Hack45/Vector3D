#pragma once

#include <optional>
#include <string_view>

#include "input/InputDevice.hpp"

namespace v3d {
namespace input {

// Source of truth for bindable keys: each entry emits the constexpr constant
// and the name-table entry. The argument is both the GLFW_KEY_* suffix and the
// persisted name.
#define V3D_INPUT_KEYS(V3D_X)                                                  \
    V3D_X(SPACE) V3D_X(APOSTROPHE) V3D_X(COMMA) V3D_X(MINUS) V3D_X(PERIOD)     \
    V3D_X(SLASH)                                                               \
    V3D_X(0) V3D_X(1) V3D_X(2) V3D_X(3) V3D_X(4) V3D_X(5) V3D_X(6) V3D_X(7)    \
    V3D_X(8) V3D_X(9)                                                          \
    V3D_X(SEMICOLON) V3D_X(EQUAL)                                              \
    V3D_X(A) V3D_X(B) V3D_X(C) V3D_X(D) V3D_X(E) V3D_X(F) V3D_X(G) V3D_X(H)    \
    V3D_X(I) V3D_X(J) V3D_X(K) V3D_X(L) V3D_X(M) V3D_X(N) V3D_X(O) V3D_X(P)    \
    V3D_X(Q) V3D_X(R) V3D_X(S) V3D_X(T) V3D_X(U) V3D_X(V) V3D_X(W) V3D_X(X)    \
    V3D_X(Y) V3D_X(Z)                                                          \
    V3D_X(LEFT_BRACKET) V3D_X(BACKSLASH) V3D_X(RIGHT_BRACKET)                  \
    V3D_X(GRAVE_ACCENT) V3D_X(WORLD_1) V3D_X(WORLD_2)                          \
    V3D_X(ESCAPE) V3D_X(ENTER) V3D_X(TAB) V3D_X(BACKSPACE) V3D_X(INSERT)       \
    V3D_X(DELETE) V3D_X(RIGHT) V3D_X(LEFT) V3D_X(DOWN) V3D_X(UP)               \
    V3D_X(PAGE_UP) V3D_X(PAGE_DOWN) V3D_X(HOME) V3D_X(END)                     \
    V3D_X(CAPS_LOCK) V3D_X(SCROLL_LOCK) V3D_X(NUM_LOCK) V3D_X(PRINT_SCREEN)    \
    V3D_X(PAUSE)                                                               \
    V3D_X(F1) V3D_X(F2) V3D_X(F3) V3D_X(F4) V3D_X(F5) V3D_X(F6) V3D_X(F7)      \
    V3D_X(F8) V3D_X(F9) V3D_X(F10) V3D_X(F11) V3D_X(F12) V3D_X(F13)            \
    V3D_X(F14) V3D_X(F15) V3D_X(F16) V3D_X(F17) V3D_X(F18) V3D_X(F19)          \
    V3D_X(F20) V3D_X(F21) V3D_X(F22) V3D_X(F23) V3D_X(F24) V3D_X(F25)          \
    V3D_X(KP_0) V3D_X(KP_1) V3D_X(KP_2) V3D_X(KP_3) V3D_X(KP_4) V3D_X(KP_5)    \
    V3D_X(KP_6) V3D_X(KP_7) V3D_X(KP_8) V3D_X(KP_9) V3D_X(KP_DECIMAL)          \
    V3D_X(KP_DIVIDE) V3D_X(KP_MULTIPLY) V3D_X(KP_SUBTRACT) V3D_X(KP_ADD)       \
    V3D_X(KP_ENTER) V3D_X(KP_EQUAL)                                            \
    V3D_X(LEFT_SHIFT) V3D_X(LEFT_CONTROL) V3D_X(LEFT_ALT) V3D_X(LEFT_SUPER)    \
    V3D_X(RIGHT_SHIFT) V3D_X(RIGHT_CONTROL) V3D_X(RIGHT_ALT)                   \
    V3D_X(RIGHT_SUPER)                                                         \
    V3D_X(MENU)

// Persisted names need the GP_ / GP_AXIS_ prefix: gamepad codes overlap
// keyboard codes. CROSS/CIRCLE/SQUARE/TRIANGLE stay out, they alias A/B/X/Y.
#define V3D_GAMEPAD_BUTTONS(V3D_X)                                             \
    V3D_X(A) V3D_X(B) V3D_X(X) V3D_X(Y)                                        \
    V3D_X(LEFT_BUMPER) V3D_X(RIGHT_BUMPER) V3D_X(BACK) V3D_X(START)            \
    V3D_X(GUIDE) V3D_X(LEFT_THUMB) V3D_X(RIGHT_THUMB)                          \
    V3D_X(DPAD_UP) V3D_X(DPAD_RIGHT) V3D_X(DPAD_DOWN) V3D_X(DPAD_LEFT)

#define V3D_GAMEPAD_AXES(V3D_X)                                                \
    V3D_X(LEFT_X) V3D_X(LEFT_Y) V3D_X(RIGHT_X) V3D_X(RIGHT_Y)                  \
    V3D_X(LEFT_TRIGGER) V3D_X(RIGHT_TRIGGER)

namespace key {

#define V3D_INPUT_KEY_CONSTANT(name) \
    constexpr InputKey IK_##name{InputKeyKind::Keyboard, GLFW_KEY_##name};
V3D_INPUT_KEYS(V3D_INPUT_KEY_CONSTANT)
#undef V3D_INPUT_KEY_CONSTANT

// Outside V3D_INPUT_KEYS, not bindable
constexpr InputKey IK_UnknownKey{InputKeyKind::Keyboard, GLFW_KEY_UNKNOWN};
constexpr InputKey IK_LAST{InputKeyKind::Keyboard, GLFW_KEY_LAST};

};  // namespace key

namespace gamepad {

#define V3D_GAMEPAD_BUTTON_CONSTANT(name)  \
    constexpr InputKey IB_##name{InputKeyKind::GamepadButton, \
                                 GLFW_GAMEPAD_BUTTON_##name};
V3D_GAMEPAD_BUTTONS(V3D_GAMEPAD_BUTTON_CONSTANT)
#undef V3D_GAMEPAD_BUTTON_CONSTANT

#define V3D_GAMEPAD_AXIS_CONSTANT(name) \
    constexpr InputKey IA_##name{InputKeyKind::GamepadAxis, \
                                 GLFW_GAMEPAD_AXIS_##name};
V3D_GAMEPAD_AXES(V3D_GAMEPAD_AXIS_CONSTANT)
#undef V3D_GAMEPAD_AXIS_CONSTANT

};  // namespace gamepad

/// @brief Persisted name of a key, e.g. "SPACE" or "GP_AXIS_LEFT_X", or nullptr
/// if not bindable.
const char* keyName(InputKey key);

/// @brief Key for a persisted name, the inverse of keyName(). Empty if unknown.
std::optional<InputKey> keyFromName(std::string_view name);

/// @brief Whether an axis rests at -1 rather than 0. True for the two gamepad
/// triggers, whose value needs rescaling to [0, 1].
bool isTriggerAxis(InputKey key);

};  // namespace input

};  // namespace v3d
