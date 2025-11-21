#pragma once
#include <glm/mat4x4.hpp>

#include "neocad/core/NeoModel.hpp"
#include "neocad/ui/Colors.hpp"

namespace nc::rendering {

class CanvasRenderer2D {
   public:
    void Init();
    void Render(const core::NeoModel&, const glm::mat4& projView);

   private:
    unsigned int m_Program = 0;
    unsigned int m_Vao = 0;
    unsigned int m_Vbo = 0;
};

}  // namespace nc::rendering
