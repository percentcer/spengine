#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec2 aTexCoord;

uniform float xOffset;
uniform mat4 transform;

out vec4 vertexColor;
out vec2 uv;

void main() {
  uv = aTexCoord;
  vertexColor = vec4(aCol.rgb, 1.0);
  gl_Position = transform * vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
}
