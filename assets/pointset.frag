#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform vec4 u_Color;    // (r,g,b,a)
uniform float u_Radius;

void main() {
    float dist = length(vUV * u_Radius);

    float core = u_Radius * 0.7;
    if (dist < core) {
        FragColor = u_Color;
        return;
    }

    if (dist < u_Radius) {
        float alpha = smoothstep(u_Radius, core, dist);
        FragColor = vec4(u_Color.rgb, u_Color.a * alpha);
        return;
    }

    discard;
}
