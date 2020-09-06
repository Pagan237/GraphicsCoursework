#version 440 core

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

//directional light data
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
//float calculate_shadow(in sampler2DShadow shadow_map, in vec4 light_space_pos);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);
// Point lights being used in the scene
uniform point_light points[1];
// Spot lights being used in the scene
uniform spot_light spots[3];
//set uniform for directional light
uniform directional_light directional;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
//shadow map uniform
//uniform sampler2DShadow shadow_map;
uniform sampler2D shadow_map;


// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord_out;
// Incoming light space position
layout(location = 3) in vec4 light_space_pos;

// Outgoing colour
layout(location = 0) out vec4 colour;

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // Get distance between point light and vertex
   float d = distance(point.position, position);
  // Calculate attenuation factor
  float atten = point.constant + (point.linear*d) + (point.quadratic*(d*d));
  // Calculate light colour
  vec4 light_colour = (1/atten)*point.light_colour;
  // Calculate light dir
  vec3 light_dir = normalize(point.position - position);
  // Now use standard phong shading but using calculated light colour and direction
  float k1 = max(dot(normal, light_dir), 0.0f);
  vec4 diffuse = k1 * mat.diffuse_reflection * light_colour;
  // Calculate view direction
   view_dir = normalize(eye_pos - position);
  // Calculate half vector
  vec3 half_vec = normalize(view_dir + light_dir);
  // Calculate specular component
 float k2 = pow(max(dot(half_vec, normal), 0.0f), mat.shininess);
 vec4 specular = k2*mat.specular_reflection*light_colour;
  // Sample texture
  tex_colour = texture(tex, tex_coord_out);
  // Calculate primary colour component
  vec4 primary = diffuse+mat.emissive;
  // Calculate final colour - remember alpha
  primary.a = 1.0f;
  vec4 sample_colour = (primary*tex_colour)+specular;
  sample_colour.a = 1.0f;
  return sample_colour;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // Calculate direction to the light
  vec3 light_dir = normalize(spot.position - position);
  // Calculate distance to light
  float d = distance(spot.position, position);
  // Calculate attenuation value
  float atten = spot.constant + (spot.linear*d) + (spot.quadratic*(d*d));
  // Calculate spot light intensity
  float inten = pow(max(dot(-spot.direction, light_dir), 0.0f), spot.power);
  // Calculate light colour
  vec4 light_colour = (inten/atten)*spot.light_colour;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
   float k1 = max(dot(normal, light_dir), 0.0f);
  vec4 diffuse = k1 * mat.diffuse_reflection * light_colour;
  // Calculate view direction
  view_dir = normalize(eye_pos - position);
  // Calculate half vector
  vec3 half_vec = normalize(view_dir + light_dir);
  // Calculate specular component
  float k2 = pow(max(dot(half_vec, normal), 0.0f), mat.shininess);
  vec4 specular = k2*mat.specular_reflection*light_colour;
  // Sample texture
  tex_colour = texture(tex, tex_coord_out);
  // Calculate primary colour component
  vec4 primary = diffuse+mat.emissive;
  // Calculate final colour - remember alpha
  primary.a = 1.0f;
  vec4 sample_colour = (primary*tex_colour)+specular;
  sample_colour.a = 1.0f;
  return sample_colour;
}

void main() {
//calculate shade factor
  float shade_factor = calculate_shadow(shadow_map, light_space_pos);
  //calculate ambient
  vec4 ambient = directional.ambient_intensity * mat.diffuse_reflection;
  //calculate diffuse
  float k1 = max(dot(transformed_normal, directional.light_dir), 0.0f);
  vec4 diffuse = k1*mat.diffuse_reflection*directional.light_colour;
  // Calculate view direction
  vec3 view_dir = (normalize(eye_pos - vertex_position));
  //calculate half vector
  vec3 half_vec = normalize(view_dir + directional.light_dir);
  //calculate specular
  float k2 = pow(max(dot(transformed_normal, half_vec), 0.0f), mat.shininess);
  vec4 specular = k2*mat.specular_reflection*directional.light_colour;
  //calculate primary and secondary
  vec4 primary = ambient + diffuse;
  vec4 secondary = specular;
  //set alphas to one
  primary.a = 1.0f;
  primary.a = 1.0f;
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord_out);
  // Sum point lights
  for (int i = 0; i < 1; ++ i )
  {
	colour +=calculate_point(points[i], mat, vertex_position, transformed_normal, view_dir, tex_colour);
	colour.a = 1.0f;
  }
  // Sum spot lights
  for (int i = 0; i < 3; ++ i ){
	vec4 lc = calculate_spot(spots[i], mat, vertex_position, transformed_normal, view_dir, tex_colour);
	if(i == 2)
	{
		lc*= shade_factor;
	}
	colour +=lc;
  }
 // colour = vec4(shade_factor,0,0,1);
}