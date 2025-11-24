#pragma once
#include <variant>

namespace nc {

/// High-level input event types, independent from GLFW/ImGui/etc.
enum class InputEventType {
    Key,
    MouseButton,
    MouseMove
};

struct KeyEvent {
    int key = 0;
    bool pressed = false;
};

struct MouseButtonEvent {
    int button = 0;  ///< e.g. 0 = left, 1 = right
    bool pressed = false;
    double x = 0.0;  ///< world or logical coordinates (decided by caller)
    double y = 0.0;
};

struct MouseMoveEvent {
    double x = 0.0;
    double y = 0.0;
};

using InputEventData = std::variant<KeyEvent, MouseButtonEvent, MouseMoveEvent>;

/// Generic input event wrapper used by the editor.
struct InputEvent {
    InputEventType type{InputEventType::Key};
    InputEventData data;
};

}  // namespace nc
