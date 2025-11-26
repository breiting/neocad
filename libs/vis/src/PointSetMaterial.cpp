#include <neocad/vis/Light.hpp>
#include <neocad/vis/PointSetMaterial.hpp>
#include <neocad/vis/Shader.hpp>

#include "assets/pointset_frag.h"
#include "assets/pointset_vert.h"

namespace nc::vis {

PointSetMaterial::PointSetMaterial() {
    m_Shader = std::make_shared<Shader>(pointset_vert_glsl, pointset_frag_glsl);
}

void PointSetMaterial::Apply(const glm::mat4& /*model*/, const glm::mat4& view, const glm::mat4& projection,
                             std::shared_ptr<Light> /*light*/) {
    m_Shader->Bind();
    m_Shader->SetMat4("u_View", view);
    m_Shader->SetMat4("u_Projection", projection);

    // uniforms
    m_Shader->SetFloat("u_Radius", m_Radius);
    m_Shader->SetVec4("u_Color", m_Color);
}

}  // namespace nc::vis
