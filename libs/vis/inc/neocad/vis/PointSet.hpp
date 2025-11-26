#pragma once
#include <glm/glm.hpp>
#include <neocad/vis/BaseGeometry.hpp>

namespace nc::vis {

class PointSet : public BaseGeometry {
   public:
    PointSet();
    ~PointSet();

    void Upload() override;
    void Render() const override;

   private:
    void deleteBuffers();

   private:
    unsigned int m_Vao = 0;
    unsigned int m_Vbo = 0;
};
}  // namespace nc::vis
