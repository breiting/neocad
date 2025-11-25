#pragma once

#include <glm/glm.hpp>
#include <neocad/rendering/BaseGeometry.hpp>

namespace nc::vis {

class LineSet : public BaseGeometry {
   public:
    LineSet();
    ~LineSet();

    void Upload() override;
    void Render() const override;

   private:
    void deleteBuffers();

   private:
    unsigned int m_Vao, m_Vbo;
};
}  // namespace nc
