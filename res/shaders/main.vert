#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec2 aTexCoord;

uniform float xOffset;

out vec4 vertexColor;
out vec2 uv;

void main() {
  gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
  vertexColor = vec4(aCol.rgb, 1.0);
  uv = aTexCoord;
}
