#pragma once
#include <variant>

namespace nc {

// -----------------------------------------
// ENUMS (no GLFW dependency!)
enum class MouseButton {
    Left,
    Right,
    Middle
};

enum class Key {
    Enter,
    Escape,
    Unknown
};

enum class KeyAction {
    Press,
    Release
};

// -----------------------------------------
// EVENT DATA TYPES

struct MouseMoveEvent {
    double x;
    double y;
};

struct MouseButtonEvent {
    MouseButton button;
    double x;
    double y;
};

struct KeyEvent {
    Key key;
    KeyAction action;
};

// -----------------------------------------
// VARIANT EVENT TYPE
using EventVariant = std::variant<MouseMoveEvent, MouseButtonEvent, KeyEvent>;

class InputEvent {
   public:
    explicit InputEvent(const MouseMoveEvent& e) : m_Data(e) {
    }
    explicit InputEvent(const MouseButtonEvent& e) : m_Data(e) {
    }
    explicit InputEvent(const KeyEvent& e) : m_Data(e) {
    }

    bool IsMouseMove() const {
        return std::holds_alternative<MouseMoveEvent>(m_Data);
    }
    bool IsMouseButton() const {
        return std::holds_alternative<MouseButtonEvent>(m_Data);
    }
    bool IsKey() const {
        return std::holds_alternative<KeyEvent>(m_Data);
    }

    bool IsLeftMouseClick() const {
        if (!IsMouseButton())
            return false;
        const auto& e = std::get<MouseButtonEvent>(m_Data);
        return e.button == MouseButton::Left;
    }

    bool IsKeyPressed(Key k) const {
        if (!IsKey())
            return false;
        const auto& e = std::get<KeyEvent>(m_Data);
        return e.key == k && e.action == KeyAction::Press;
    }

    const MouseMoveEvent& AsMouseMove() const {
        return std::get<MouseMoveEvent>(m_Data);
    }
    const MouseButtonEvent& AsMouseButton() const {
        return std::get<MouseButtonEvent>(m_Data);
    }
    const KeyEvent& AsKey() const {
        return std::get<KeyEvent>(m_Data);
    }

   private:
    EventVariant m_Data;
};

}  // namespace nc
