#pragma once

#include <glm/glm.hpp>

namespace nc {

class ICamera {
   public:
    virtual ~ICamera() = default;

    /// Set the render surface aspect ratio (width / height).
    virtual void SetAspect(float aspect) = 0;

    /// Get the 4×4 view matrix (world → view).
    virtual glm::mat4 View() const = 0;

    /// Get the 4×4 projection matrix (view → clip).
    virtual glm::mat4 Projection() const = 0;
};
}  // namespace nc
