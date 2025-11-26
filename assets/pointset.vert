#version 330 core

layout(location = 0) in vec3 iPos;

// QUAD corner
vec2 getCorner(int id) {
    if (id == 0) return vec2(-1, -1);
    if (id == 1) return vec2( 1, -1);
    if (id == 2) return vec2(-1,  1);
    return vec2( 1,  1);
}

uniform mat4 u_View;
uniform mat4 u_Proj;
uniform float u_Radius;

out vec2 vUV;

void main() {
    vUV = getCorner(gl_VertexID);
    vec3 pos = iPos + vec3(vUV * u_Radius, 0.0);
    gl_Position = u_Proj * u_View * vec4(pos, 1.0);
}
