#version 330 core

out vec4 FragColor;

in vec4 vertexColor;
in vec2 uv;

uniform float scaling;
uniform sampler2D tex1;

void main() { FragColor = vec4(vertexColor * scaling) * texture(tex1, uv); }
