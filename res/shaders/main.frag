#version 330 core

out vec4 FragColor;
in vec4 vertexColor;

uniform float scaling;

void main()
{
    FragColor = vec4(vertexColor * scaling);
}
