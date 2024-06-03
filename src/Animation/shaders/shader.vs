#version 420 core
layout (location = 0) in vec3 position;
uniform vec3 color;
uniform mat4 view;
uniform float pointsize;
out vec4 vertex_color;
void main()
{
  vertex_color = vec4(color, 1.0f);
  gl_Position = view * vec4(position, 1.0f);
  gl_PointSize = pointsize;
}
