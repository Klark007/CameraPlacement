#include "CameraPlacement.h"

App* g_app = nullptr;

App::App(uint32_t res_x, uint32_t res_y, const std::string& camera_path, const std::string& model_path, const std::string& ouput_path, float fovy, float near_plane, float far_plane)
{
	this->res_x = res_x;
	this->res_y = res_y;

	glfw_init();
	glew_init();
	opengl_setup();

	load_programs();
	load_models(model_path);
	setup_camera(fovy, near_plane, far_plane);
	setup_renderpasses();

	setup_placeable_cameras(camera_path, near_plane, far_plane);

	gui = std::make_unique<GUI>(window, phong_shading_program, camera_names, camera_logos, near_plane, far_plane);
	controller = std::make_unique<Controller>(fly_camera, camera_types, camera_logos, res_x, res_y, ouput_path);
}

App::~App()
{
	gui.release();
	glfwTerminate();
}

void App::run()
{
	// for tracking delta time
	controller->init_time();

	// game loop
	while (!glfwWindowShouldClose(window))
	{
		GuiOutput gui_variables = update();

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			recompile_shaders = true;
		}
		if (recompile_shaders && glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
			std::cout << "Recompile" << std::endl;

			phong_shading_program = Program::conditional_recompile(phong_shading_program);
			post_process_program = Program::conditional_recompile(post_process_program);

			recompile_shaders = false;
		}

		draw(gui_variables);

		post_draw(gui_variables);
	}
}

GuiOutput App::update()
{
	glfwPollEvents();
	GuiOutput gui_variables = gui->get_gui_output();

	current_camera = controller->get_current_camera();
	current_camera->set_near_plane(gui_variables.camera_near_plane);
	current_camera->set_far_plane(gui_variables.camera_far_plane);

	if (resize_window) {
		resize_resources();
		resize_window = false;
	}

	controller->update_time();
	controller->handle_keys(window, placed_cameras);
	controller->set_move_strength(gui_variables.camera_movement_speed);
	controller->set_rotation_strength(gui_variables.camera_rotation_speed);
	resize_window = resize_window || controller->set_camera_type(gui_variables.selected_camera_type);

	return gui_variables;
}

void App::draw(GuiOutput input)
{
	// main pass begin
	if (!input.toggle_msaa) {
		main_pass->use();
	}
	else {
		msaa_main_pass->use();
	}

	glm::mat4 view = current_camera->generate_view_mat();
	glm::mat4 proj = current_camera->generate_projection_mat();

	// set matrices
	phong_shading_program->set_mat4f("model", glm::mat4(1.0));
	phong_shading_program->set_mat4f("view", view);
	phong_shading_program->set_mat4f("projection", proj);

	glm::mat4 vp_mat = proj * current_camera->generate_virtual_view_mat();

	// draw models
	for (std::unique_ptr<Model>& model : models) {
		model->draw(phong_shading_program, vp_mat, input.toggle_frustum_culling, input.toggle_aabb_drawing);
	}

	// draw frustums
	if (input.preview_frustums) {

		for (std::shared_ptr<CameraPreview>& preview : placed_cameras) {
			preview->set_dir(current_camera->get_dir());
			preview->draw_frustum(phong_shading_program);
		}
	}

	// draw icons
	if (input.preview_icon) {
		// overwrite the currently used programm set by renderpass
		textured_program->use();
		textured_program->set_mat4f("model", glm::mat4(1.0));
		textured_program->set_mat4f("view", view);
		textured_program->set_mat4f("projection", proj);

		for (std::shared_ptr<CameraPreview>& preview : placed_cameras) {
			preview->set_dir(current_camera->get_dir());
			preview->draw_icon(textured_program);
		}

		phong_shading_program->use();
	}

	if (input.toggle_msaa) {
		msaa_main_pass->resolve(); // blit to resolve msaa pass
	}

	// main pass end

	// post process pass begin
	postprocess_pass->use();

	if (!input.toggle_msaa) {
		main_pass->get_color_texture()->bind();
	}
	else {
		msaa_main_pass->get_color_texture()->bind();
	}

	post_process_program->set_vec2f("screen_res", res_x, res_y);
	post_process_program->set_vec2f("uv_offset", uv_offset);
	post_process_program->set_vec2f("uv_scale", uv_scale);
	post_process_program->set1i("fxaa_enabled", input.toggle_fxaa);
	post_process_program->set1i("cross.enabled", controller->can_move_round()); // to toggle cross in center of screen

	view_plane.draw();
	// post process pass end

	gui->render();
}

void App::post_draw(GuiOutput input)
{
	// if we switch to preview mode, we might need to resize the frame buffers due to different aspect ratio
	if (!input.toggle_msaa) {
		resize_window = resize_window || controller->place_camera(window, main_pass, placed_cameras, input);
	}
	else {
		resize_window = resize_window || controller->place_camera(window, msaa_main_pass, placed_cameras, input);
	}

	glfwSwapBuffers(window);
}

inline void App::glfw_init()
{
	if (!glfwInit()) {
		throw SetupException("GLFW Initalization failed", __FILE__, __LINE__);
	}
	glfwSetErrorCallback(glfw_error_callback);

	// require minimum openGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(this->res_x, this->res_y, "Camera Placement", NULL, NULL);

	if (!window) {
		glfwTerminate();
		throw SetupException("GLFW Window creation failed", __FILE__, __LINE__);
	}

	glfwMakeContextCurrent(window);

	if (!glfwRawMouseMotionSupported()) {
		glfwTerminate();
		throw SetupException("GLFW Raw mouse motion not supported", __FILE__, __LINE__);
	}

	// this toggles vsync
	glfwSwapInterval(0); // default swap interval is 0 which can lead to screen tearing
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, glfm_mouse_move_callback);
}

void App::glew_init()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		glfwTerminate();
		throw SetupException("GLEW Initation failed: " + std::string((const char*) glewGetErrorString(err)), __FILE__, __LINE__);
	}

#ifdef DEBUG
	if (!GLEW_KHR_debug) {
		glfwTerminate();
		throw SetupException("KGR_debug not supported", __FILE__, __LINE__);
	}
#endif

	std::cout << "Status: Using GLEW: " << glewGetString(GLEW_VERSION) << std::endl;
}

void App::opengl_setup()
{
#ifdef DEBUG
	// enable debug messages
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // synchronous with function call order
	glDebugMessageCallback(message_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // disable notifications
#endif
	glLineWidth(1);
	glPointSize(4);
	
	// enable for transparent icons
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// TODO: enable once received fixed asset (issue with normals in base asset)
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// glFrontFace(GL_CW);

	// reverse z
	//glDepthFunc(GL_GEQUAL);
	//glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	glEnable(GL_MULTISAMPLE);
}

void App::load_models(const std::string& model_path)
{
	for (auto const& entry : std::filesystem::directory_iterator{ model_path }) {
		models.emplace_back(std::make_unique<Model>(string_from_path(entry.path())));
	}

	// hard coded view plane
	std::vector<Vertex> vertices = {
		{glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(0.0,0.0)},
		{glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,1.0,0.0), glm::vec2(0.0,1.0)},
		{glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(1.0,0.0)},
		{glm::vec3(1.0,0.0,1.0), glm::vec3(0.0,1.0,0.0), glm::vec2(1.0,1.0)}
	};
	std::vector<Index> indices = { 0,1,2,1,3,2 };
	view_plane = { vertices, indices};
}

void App::load_programs()	
{
	std::vector<std::shared_ptr<Shader>> phong_shaders;
	phong_shaders.push_back(std::make_shared<Shader>(GL_VERTEX_SHADER, "shaders/phong.vs"));
	phong_shaders.push_back(std::make_shared<Shader>(GL_FRAGMENT_SHADER, "shaders/phong.fs"));
	phong_shading_program = std::make_shared<Program>(phong_shaders);

	std::vector<std::shared_ptr<Shader>> textured_shaders;
	textured_shaders.push_back(std::make_shared<Shader>(GL_VERTEX_SHADER, "shaders/textured.vs"));
	textured_shaders.push_back(std::make_shared<Shader>(GL_FRAGMENT_SHADER, "shaders/textured.fs"));
	textured_program = std::make_shared<Program>(textured_shaders);

	std::vector<std::shared_ptr<Shader>> postprocess_shaders;
	postprocess_shaders.push_back(std::make_shared<Shader>(GL_VERTEX_SHADER, "shaders/postprocess.vs"));
	postprocess_shaders.push_back(std::make_shared<Shader>(GL_FRAGMENT_SHADER, "shaders/postprocess.fs"));
	post_process_program = std::make_shared<Program>(postprocess_shaders);
}

inline void App::setup_camera(float fovy, float near_plane, float far_plane)
{
	fly_camera = std::make_shared<Camera>(
		glm::vec3(-0.2, 0.5, 0),
		glm::vec3(0.5, 0.5, 0),
		glm::vec3(0.0, 1.0, 0.0),
		res_x, res_y,
		fovy,
		near_plane,
		far_plane
	);

	glm::mat4 projection = fly_camera->generate_projection_mat();
}

void App::setup_renderpasses()
{
	glm::vec4 clear_color = glm::vec4(0.68, 0.68, 0.68, 1);
	float depth_clear_value = 1.0;

	main_pass = std::make_shared<Renderpass>(
		res_x, res_y,
		phong_shading_program,
		clear_color,
		true,
		depth_clear_value
	);

	msaa_main_pass = std::make_shared<MSAA_Renderpass>(
		res_x, res_y,
		phong_shading_program,
		4,
		clear_color,
		true,
		depth_clear_value
	);

	postprocess_pass = std::make_shared<Renderpass>(
		res_x, res_y,
		post_process_program,
		clear_color,
		false,
		depth_clear_value,
		true
	);

	post_process_program->use();
	main_pass->get_color_texture()->set_texture_unit(*post_process_program, 0);
	msaa_main_pass->get_color_texture()->set_texture_unit(*post_process_program, 0);
}

void App::setup_placeable_cameras(const std::string& cameras_path, float near_plane, float far_plane)
{
	std::cout << "Cameras found:" << std::endl;
	for (auto const& entry : std::filesystem::directory_iterator{ cameras_path }) {
		if (std::filesystem::is_directory(entry)) {
			std::filesystem::path path = entry.path();
			camera_names.emplace_back(string_from_path(path.filename()));
			std::cout << path.filename() << std::endl;

			std::shared_ptr<Texture> icon = create_texture_from_file("", string_from_path(path / "icon.png"));
			camera_logos.emplace_back(icon);

			rapidcsv::Document intrinsics_doc(string_from_path(path / "intrinsics.csv"), rapidcsv::LabelParams(-1, -1));
			std::vector<float> intrinsics = intrinsics_doc.GetRow<float>(0);
			camera_types.emplace_back(std::make_shared<Camera>(
				glm::vec3(0, 0, 0),
				glm::vec3(1, 0, 0),
				glm::vec3(0, 1, 0),
				intrinsics[2] * 2, intrinsics[3] * 2,
				intrinsics[0],
				intrinsics[1],
				near_plane,
				far_plane
			));
		}
	}
}

void App::resize_resources()
{
	// fly camera changes with the main view
	fly_camera->set_aspect_ratio(res_x, res_y);

	float camera_aspect_ratio = current_camera->get_aspect_ratio();

	// if aspect ratio cam < screen aspect ration then we have borders on the left and right
	// otherwise on top and bottom
	if (camera_aspect_ratio < (float) res_x / res_y) {
		// linear scaling such that cam_res_y corresponds to res_y
		float border_width = (res_x - (camera_aspect_ratio * res_y)) / 2;
		uv_offset = glm::vec2(border_width / res_x, 0);
		uv_scale = glm::vec2(camera_aspect_ratio * res_y / res_x, 1); 
	}
	else {
		// linear scaling such that cam_res_x corresponds to res_x
		float border_width = (res_y - (1 / camera_aspect_ratio * res_x)) / 2;
		uv_offset = glm::vec2(0, border_width / res_y);
		uv_scale = glm::vec2(1, 1 / camera_aspect_ratio * res_x / res_y);
	}

	// cameras resolution matters for main pass, postprocess should be done with res_x, res_y and an uv offset
	unsigned int cam_res_x = current_camera->get_resolution_x();
	unsigned int cam_res_y = current_camera->get_resolution_y();

	main_pass->resize(cam_res_x, cam_res_y);
	msaa_main_pass->resize(cam_res_x, cam_res_y);
	
	postprocess_pass->resize(res_x, res_y);
}


int main(int argc, char* argv[])
{
	// set path
	std::filesystem::path cwd = std::filesystem::current_path();
	cwd = cwd.parent_path().parent_path().parent_path();
	std::filesystem::current_path(cwd);

	std::cout << "CWD:" << std::filesystem::current_path() << std::endl;


	// parse arguments 
	// document with https://tree.nathanfriend.io/
	argparse::ArgumentParser program("Camera Placement");

	program.add_argument("-c", "-cameras")
		.default_value("cameras")
		.required()
		.help("Path to folder where camera icon, intrinics and optionally transforms (for stereo cameras) are stored");

	program.add_argument("-m", "-models")
		.default_value("models\\default")
		.required()
		.help("Path to folder where models to be used are stored");

	program.add_argument("-o", "-output")
		.default_value("output")
		.required()
		.help("Path to folder where camera intrinics and extrinics are exported to");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}

	// start application
	try {
		App app{ 800, 600, program.get("-c"), program.get("-m"), program.get("-o"), glm::radians(45.0), 0.1, 25.0};
		g_app = &app;

		app.run();
	}
	catch (const CameraPlacementException& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}

// call apps appropriate function
void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	if (g_app != nullptr) {
		g_app->should_resize(width, height);
	}
}

// call controllers appropriate function
void glfm_mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (g_app != nullptr) {
		g_app->controller->handle_mouse(xpos, ypos);
	}
}


// opengl error handling
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
		}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
		}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
		}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}