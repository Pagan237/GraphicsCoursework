#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace std::chrono;
using namespace graphics_framework;
using namespace glm;

map <string, mesh> meshes;
map <string, texture> textures;
mesh water;
texture water_tex;
texture normal_map;
texture mask;
geometry screen_quad;
effect shadow_eff;
effect eff;
effect normal_eff;
effect mask_eff;
effect grey_eff;
target_camera tar_cam;
free_camera free_cam;
vector<spot_light> spots(3);
vector<point_light> points(1);
frame_buffer framebuf;
directional_light dir;
shadow_map shadow;
double cursor_x = 0.0f;
double cursor_y = 0.0f;
bool cameras = true;
int m = 0;
int grey = 0;

bool initialise() {
	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Capture initial mouse position

	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	return true;
}

bool load_content() {

	// Create plane
	meshes["plane"] = mesh(geometry_builder::create_plane());
	//Create scene + transform objects
	meshes["saloon"] = mesh(geometry("models/saloon.obj"));
	meshes["saloon"].get_transform().scale = vec3(0.1f, 0.1f, 0.1f);
	meshes["saloon"].get_transform().translate(vec3(-20.0f, 0.00001f, -20.0f));
	meshes["railway"] = mesh(geometry("models/railway.obj"));
	meshes["railway"].get_transform().scale = vec3(0.001f, 0.001f, 0.00308f); 
	meshes["bar"] = mesh(geometry("models/bar.obj"));
	meshes["bar"].get_transform().scale = vec3(0.5f, 0.2f, 0.3f);
	meshes["bar"].get_transform().translate(vec3(-23.0f, 0.9f, -20.7f));
	meshes["barrel"] = mesh(geometry("models/barrel.obj"));
	meshes["barrel"].get_transform().translate(vec3(-31.25f, 0.5f, -18.2f));
	meshes["barrel"].get_transform().scale = vec3(0.01f, 0.01f, 0.01f);
	meshes["barrel"].get_transform().rotate(vec3(0.0f, pi<float>(), 0.0f));
	meshes["gun"] = mesh(geometry("models/revolver.obj"));
	meshes["gun"].get_transform().translate(vec3(-31.25f, 1.33f, -18.3f));
	meshes["gun"].get_transform().scale = vec3(0.003f, 0.003f, 0.003f); 
	meshes["gun"].get_transform().rotate(vec3(0.0f, 0.0f, half_pi<float>())); 
	meshes["sun"] = mesh(geometry_builder::create_sphere());   
	meshes["sun"].get_transform().translate(vec3(0.0f, 30.0f, 0.0f));
	meshes["sun"].get_transform().scale = vec3(-1.0f, -1.0f, -1.0f);
	meshes["train"] = mesh(geometry("models/train.3DS"));
	meshes["train"].get_transform().scale = vec3(0.001f, 0.001f, 0.001f);
	meshes["train"].get_transform().rotate(vec3(half_pi<float>(), half_pi<float>(), pi<float>()));
	meshes["train"].get_transform().translate(vec3(0.6f, 0.0f, -35.0f));
	meshes["well"] = mesh(geometry("models/well.obj"));
	meshes["well"].get_transform().translate(vec3(25.0f, 0.0f, 5.0f));  
	meshes["well"].get_transform().scale = vec3(0.01f, 0.01f, 0.01f);
	//load textures
	textures["saloon"] = texture("textures/saloon_defuse.jpg");
	textures["plane"] = texture("textures/wild_west.jpg");
	textures["railway"] = texture("textures/track.jpg");
	textures["bar"] = texture("textures/bar.png");
	textures["barrel"] = texture("textures/barrel.jpg");
	textures["gun"] = texture("textures/revolver.jpg"); 
	textures["sun"] = texture("textures/sun.jpg");
	textures["train"] = texture("textures/train.jpg");
	textures["well"] = texture("textures/well.jpg");
	//set materials
	material mat;
	mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	mat.set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mat.set_shininess(100.0f);
	mat.set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["saloon"].set_material(mat);
	meshes["sun"].set_material(mat);
	meshes["plane"].set_material(mat);
	//set light values 
	//ambient intesity
	dir.set_ambient_intensity(vec4(0.3f, 0.3f, 0.3f, 1.0f));
	//position
	spots[0].set_position(vec3(-22.0f, 10.0f, -22.0f)); 
	points[0].set_position(meshes["sun"].get_transform().position);
	spots[1].set_position(vec3(meshes["train"].get_transform().position.x, 4.0f, meshes["train"].get_transform().position.z));
	spots[2].set_position(vec3(25.0f, 5.0f, 7.0f));
	//direction
	spots[0].set_direction(normalize(vec3(0.0f, -1.0f, 0.0f)));
	spots[1].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
	spots[2].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
	dir.set_direction(normalize(vec3(1.0f, 1.0f, -1.0f)));
	//colour
    spots[0].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	points[0].set_light_colour(vec4(1.0f, 0.8f, 0.0f, 1.0f));
	spots[1].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spots[2].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	dir.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//range
	spots[0].set_range(50.0f);
	points[0].set_range(100.0f); 
	spots[1].set_range(20.0f);
	spots[2].set_range(30.0f);
	//power
	spots[0].set_power(1.0f);
	spots[1].set_power(1.0f);
	spots[2].set_power(1.0f);
	//create mesh for normal map
	water = mesh(geometry_builder::create_disk(20.0f));
	//transform object 
	water.get_transform().scale = (vec3(1.2f, 1.0f, 1.2f));
	water.get_transform().translate(vec3(25.0f, 1.0f, 5.0f));
	//define materials 
	water.get_material().set_shininess(10.0f);
	water.get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	water.get_material().set_diffuse(vec4(0.5f, 0.5f, 1.0f, 1.0f));
	//load normal and regular textures
	water_tex = texture("textures/water.jpg"); 
	normal_map = texture("textures/normal_water.jpg"); 
	framebuf = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	//Create screen quad
	vector<vec3> positions{vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f)};
	vector<vec2> tex_coords{vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f)};
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_TRIANGLE_STRIP);
	//set texture for mask
	mask = texture("textures/mask.jpg");
	//create frame buffer for screenshot
	//load shaders
	normal_eff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
	normal_eff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
	normal_eff.add_shader("shaders/part_normal_map.frag", GL_FRAGMENT_SHADER);
	normal_eff.add_shader("shaders/part_direction.frag", GL_FRAGMENT_SHADER);
	//Build normal effect
	normal_eff.build();
	//create shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
	//add shaders for shadow map
	shadow_eff.add_shader("shaders/basic.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("shaders/basic.frag", GL_FRAGMENT_SHADER); 
	shadow_eff.build();
	//shaders for regular effects
	eff.add_shader("shaders/multi-light.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("shaders/multi-light.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/part_shadow.frag", GL_FRAGMENT_SHADER);
	// Build effect    
	eff.build();
	//add shaders for post processing
	mask_eff.add_shader("shaders/post.vert", GL_VERTEX_SHADER);
	mask_eff.add_shader("shaders/mask.frag", GL_FRAGMENT_SHADER);
	mask_eff.build();
	grey_eff.add_shader("shaders/post.vert", GL_VERTEX_SHADER);
	grey_eff.add_shader("shaders/greyscale.frag", GL_FRAGMENT_SHADER);
	grey_eff.build();
	//update shadow lights
	shadow.light_dir = spots[2].get_direction();
	shadow.light_position = spots[2].get_position();
	// Set camera properties
	tar_cam.set_position(vec3(50.0f, 50.0f, 50.0f));
	tar_cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	tar_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	free_cam.set_position(vec3(0.0f, 10.0f, 0.0f));
	free_cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	free_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
};

bool update(float delta_time) {
	//Ratio of pixels to rotation
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() *
		(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
		static_cast<float>(renderer::get_screen_height());

	double current_x;
	double current_y;
	//get the cursor position
	current_x = 0;
	current_y = 0;
	//Calculate delta of cursor positions from last frame
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	//multiply deltas by ratios
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;
	//rotate camera around delta
	delta_x = delta_x*ratio_width;
	delta_y = delta_y*ratio_height;
	free_cam.rotate(delta_x, -delta_y);

	//range of light
	static float s_range = 50.0f;
	static float p_range = 100.0f;
	// Use keyboard to move the  free camera - WSAD
	if (glfwGetKey(renderer::get_window(), 'W')) {
		free_cam.move(vec3(0.0f, 0.0f, 0.25f));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), 'A')) {
		free_cam.move(vec3(-0.25f, 0.0f, 0.0f));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), 'S')) {
		free_cam.move(vec3(0.0f, 0.0f, -0.25));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), 'D')) {
		free_cam.move(vec3(0.25f, 0.0f, 0.0f));
		cameras = true;
	}
	//Use keyboard to update target camera - Z,X,C
	if (glfwGetKey(renderer::get_window(), 'Z')) {
		tar_cam.set_position(vec3(50.0f, 10.0f, 50.0f));
		cameras = false;
	}
	if (glfwGetKey(renderer::get_window(), 'X')) {
		tar_cam.set_position(vec3(-25.0f, 10.0f, -30.0f));
		cameras = false;
	}
	if (glfwGetKey(renderer::get_window(), 'C')) {
		tar_cam.set_position(vec3(-24.0f, 5.0f, -23.0f));
		cameras = false;
	}

	//O and P to turn light on and off
	if (glfwGetKey(renderer::get_window(), 'O')) {
		s_range = 50.0f;
		spots[0].set_range(s_range);
	}
	if (glfwGetKey(renderer::get_window(), 'P')){
		s_range = 0.0f;
		spots[0].set_range(s_range); 
	}
	// 0 and 1 to turn mask on and off
	if (glfwGetKey(renderer::get_window(), '0')) {
		m = 0;
	}
	if (glfwGetKey(renderer::get_window(), '1')) {
		m = 1;
		grey = 0;
	}
	//2 and 3 to turn grey scale on and off
	if (glfwGetKey(renderer::get_window(), '2')) {
		grey = 0;
	}
	if (glfwGetKey(renderer::get_window(), '3')) {
		grey = 1;
		m = 0;
	}
	//move sun across map
	static float total_time = 0.0f;
	total_time += delta_time;
	meshes["sun"].get_transform().position.x = 50*cos(total_time/2.5f);
	meshes["sun"].get_transform().position.y = 30*sin(total_time/2.5f)-2;
	points[0].set_position(meshes["sun"].get_transform().position);
	points[0].set_light_colour(vec4(sin(total_time/2.5f), sin(total_time/2.5f), 0.0f, 1.0f));
	if (meshes["sun"].get_transform().position.y < -2)
		total_time = 0.0f; 
	//move train
		meshes["train"].get_transform().position.z += 0.183f;
		if (meshes["train"].get_transform().position.z >= 52)
			meshes["train"].get_transform().position.z = -35.0f;
		spots[1].set_position(meshes["train"].get_transform().position);
		spots[1].set_position(vec3(meshes["train"].get_transform().position.x, 4.0f, meshes["train"].get_transform().position.z));

	//Load texture
	// Update the cameras
    tar_cam.update(delta_time);
	free_cam.update(delta_time);
	cursor_x = current_x;
	cursor_y = current_y;
	return true;
}

void normal_render()
{
	//bind normal effect
	renderer::bind(normal_eff);
	//create MVP
	auto M = water.get_transform().get_transform_matrix();
	auto V = free_cam.get_view();
	auto P = free_cam.get_view();
	if (cameras == true)
	{
		V = free_cam.get_view();
		P = free_cam.get_projection();
	}
	else 
	{
		V = tar_cam.get_view();
		P = tar_cam.get_projection();
	}
	auto MVP = P*V*M;
	// Set MVP Uniform Matrix
	glUniformMatrix4fv(normal_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	//M matrix
	glUniformMatrix4fv(normal_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	//N matrix
	glUniformMatrix3fv(normal_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(water.get_transform().get_normal_matrix()));
	//bind material
	renderer::bind(water.get_material(), "mat");
	//bind lights
	renderer::bind(dir, "light");
	//bind texture
	renderer::bind(water_tex, 0);
	//tex uniform
	glUniform1i(normal_eff.get_uniform_location("tex"), 0);
	//bind normal map
	renderer::bind(normal_map, 1);
	//set normal map uniform location
	glUniform1i(normal_eff.get_uniform_location("normal_map"), 1);
	//Set eye position using active camera 
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(free_cam.get_position()));
	//render water
	renderer::render(water);

}

void shadow_render() {
	//set render target to shadow map
	renderer::set_render_target(shadow);
	glEnable(GL_DEPTH_TEST);
	//clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
	//render mode to cull face
	glCullFace(GL_FRONT);
	//bind shader
	mat4 LightProjMat = perspective<float>(90.0f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
	renderer::bind(shadow_eff);
	for (auto &e : meshes) {
		auto m = e.second;
		//MVP
		auto M = m.get_transform().get_transform_matrix();
		//view from shadow map, not camera
		auto V = shadow.get_view();  
		auto MVP = LightProjMat * V * M;
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		renderer::render(m);
		}
		//set render target back to screen
		renderer::set_render_target();
		//cull to back
		glCullFace(GL_BACK);
}

void rendering() {
	if (cameras && (m == 1 || grey== 1))
	{
		//set render target to frame buffer
		renderer::set_render_target(framebuf);
		//clear frame
		renderer::clear();
	}
	glEnable(GL_DEPTH_TEST);
	mat4 LightProjMat = perspective<float>(90.0f, renderer::get_screen_aspect(), 0.1f, 1000.0f);
	renderer::bind(eff);
	renderer::bind(spots, "spots");
	renderer::bind(points, "points");
	renderer::bind(dir, "directional");
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		///renderer::bind(eff);
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = free_cam.get_view();
		auto P = free_cam.get_view();
		if (cameras)
		{
			V = free_cam.get_view();
			P = free_cam.get_projection();
		}
		else
		{
			V = tar_cam.get_view();
			P = tar_cam.get_projection();
		}
		auto MVP = P * V * M;
		//set light transform matrix
		auto LM = m.get_transform().get_transform_matrix();
		auto LV = shadow.get_view();
		auto LightMVP = LightProjMat*LV*LM;
		glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(LightMVP));
		// Set MVP matrix uniform 
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		//Set M matrix unifrom
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));  
		//set N matrix Uniform
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		//bind material
		renderer::bind(m.get_material(), "mat");
		//bind textures 
		renderer::bind(textures[e.first], 0);
		//set texture uniform
		glUniform1i(eff.get_uniform_location("tex"), 0);
		//Set eye position using active camera 
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(free_cam.get_position()));
		renderer::bind(shadow.buffer->get_depth(), 4);
		auto rr = eff.get_uniform_location("shadow_map");
		glUniform1i(eff.get_uniform_location("shadow_map"), 4);

		//disables cull face for saloon so don't see through walls
		if (e.first == "saloon") {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
		}

		// Render mesh
		renderer::render(m);
	}
	if (cameras && m == 1)
	{
		//set render target back to screen
		renderer::set_render_target();
		//bind post_eff
		renderer::bind(mask_eff);
		//MVP is identity matrix
		auto MVP = mat4(1.0f);
		//set MVP matrix uniform
		glUniformMatrix4fv(mask_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		//bind texture to frame buffer
		renderer::bind(framebuf.get_frame(), 0);
		//set tex unifrom
		glUniform1i(mask_eff.get_uniform_location("tex"), 0);
		//bind mask to alpha map
		renderer::bind(mask, 1);
		glUniform1i(mask_eff.get_uniform_location("alpha_map"), 1);
		//render screen quad
		renderer::render(screen_quad);
	}
	if (cameras && grey == 1)
	{
		renderer::set_render_target();
		renderer::bind(grey_eff);
		//MVP is identity matrix
		auto MVP = mat4(1.0f);
		glUniformMatrix4fv(grey_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		//bind frame buffer to texture
		renderer::bind(framebuf.get_frame(), 1);
		glUniform1i(grey_eff.get_uniform_location("tex"), 1);
		renderer::render(screen_quad);
	}
}
bool render()
{
	shadow_render();
	rendering();
	normal_render();
	return true;
}



void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}