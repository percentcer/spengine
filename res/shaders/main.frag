#version 330 core

out vec4 FragColor;

in vec4 vertexColor;
in vec2 uv;

uniform float scaling;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() { 
        FragColor = mix(texture(tex0, uv), texture(tex1, uv), scaling);
    }
