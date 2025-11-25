#pragma once

#include <glm/glm.hpp>
#include <neocad/vis/BaseGeometry.hpp>

namespace nc::vis {

class PointCloud : public BaseGeometry {
   public:
    PointCloud();
    ~PointCloud();

    void Upload() override;
    void Render() const override;

   private:
    void deleteBuffers();

   private:
    unsigned int m_Vao, m_Vbo;
};
}  // namespace nc::vis
