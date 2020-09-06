#version 440

// Model transformation matrix
uniform mat4 M;
// Transformation matrix
uniform mat4 MVP;
// Normal matrix
uniform mat3 N;
//light transformation matrix
uniform mat4 lightMVP;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Incoming texture coordinate
layout(location = 10) in vec2 tex_coord_in;

// Outgoing position
layout(location = 0) out vec3 vertex_position;
// Outgoing transformed normal
layout(location = 1) out vec3 transformed_normal;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord_out;
layout (location = 3) out vec4 light_space_pos;

void main() {
/*
const mat4 lightbias = mat4(
    vec4(0.5f, 0.0f, 0.0f, 0.0f),
    vec4(0.0f, 0.5f, 0.0f, 0.0f),
    vec4(0.0f, 0.0f, 0.5f, 0.0f),
    vec4(0.5f, 0.5f, 0.5f, 1.0f)
  );*/
  // Calculate screen position
  gl_Position = MVP * vec4(position, 1.0);
  // Transform position to world space
  vertex_position = vec3(M*vec4(position,1.0f));
  //transform normal
  transformed_normal = N * normal;
  //Pass through texture coordinate
  tex_coord_out = tex_coord_in;
  light_space_pos = lightMVP*(vec4(position, 1.0f));
  //light_space_pos = lightbias * lightMVP * vec4(position, 1.0);
}