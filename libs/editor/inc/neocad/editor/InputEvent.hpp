#pragma once
#include <glm/vec2.hpp>
#include <neocad/domain/Types.hpp>
#include <variant>

namespace nc {

enum class InputEventType {
    Key,
    MouseButton,
    MouseMove,
    Scroll
};

enum class MouseButton {
    Left,
    Right,
    Middle
};

enum class KeyCode {
    Unknown,
    Escape,
    Enter
};

struct KeyEvent {
    KeyCode code = KeyCode::Unknown;
    bool pressed = false;
    char text = 0;  ///< ASCII-Code
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
};

struct MouseButtonEvent {
    MouseButton button = MouseButton::Left;
    bool pressed = false;
    vec2 position{0.0, 0.0};
};

struct MouseMoveEvent {
    vec2 position{0.0, 0.0};
};

struct ScrollEvent {
    vec2 offset{0.0, 0.0};
};

using InputEventData = std::variant<KeyEvent, MouseButtonEvent, MouseMoveEvent, ScrollEvent>;

struct InputEvent {
    InputEventType type;
    InputEventData data;
};

// Convenience Helfer

inline const KeyEvent* AsKey(const InputEvent& ev) {
    if (ev.type != InputEventType::Key)
        return nullptr;
    return std::get_if<KeyEvent>(&ev.data);
}

inline const MouseButtonEvent* AsMouseButton(const InputEvent& ev) {
    if (ev.type != InputEventType::MouseButton)
        return nullptr;
    return std::get_if<MouseButtonEvent>(&ev.data);
}

inline const MouseMoveEvent* AsMouseMove(const InputEvent& ev) {
    if (ev.type != InputEventType::MouseMove)
        return nullptr;
    return std::get_if<MouseMoveEvent>(&ev.data);
}

inline const ScrollEvent* AsScroll(const InputEvent& ev) {
    if (ev.type != InputEventType::Scroll)
        return nullptr;
    return std::get_if<ScrollEvent>(&ev.data);
}

}  // namespace nc
