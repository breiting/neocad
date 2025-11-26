#pragma once
#include <glm/mat4x4.hpp>

namespace nc::editor {

class ICamera {
   public:
    virtual ~ICamera() = default;

    virtual glm::mat4 GetViewMatrix() const = 0;
    virtual glm::mat4 GetProjectionMatrix() const = 0;
    virtual void SetAspectRatio(float aspect) = 0;

    virtual void OnMouseStart() = 0;
    virtual void OnMouseRotation(double x, double y) = 0;
    virtual void OnMousePan(double x, double y) = 0;
    virtual void OnMouseScroll(double yoffset) = 0;

    virtual void Update(float dt) = 0;
};

}  // namespace nc::editor
