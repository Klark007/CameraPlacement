// CameraPlacement.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rapidcsv.h"
#include "argparse.hpp"

#include "engine/Exception.h"

#include "engine/Program.h"
#include "engine/Shader.h"
#include "engine/Renderpass.h"
#include "engine/Multisample_Renderpass.h"

#include "engine/Model.h"
#include "engine/Mesh.h"

#include "Gui.h"
#include "Controller.h"

#include "engine/Helper.h"

#include <iostream>
#include <filesystem>
#include <array>
#include <string>

class App {
public:
	App(uint32_t res_x, uint32_t res_y, const std::string& camera_path, const std::string& model_path, const std::string& ouput_path , float fovy = glm::radians(45.0), float near_plane = 0.1, float far_plane = 200.0);
	~App();
	void run();
private:
	GLFWwindow* window;
	uint32_t res_x, res_y;

	std::unique_ptr<GUI> gui;

	// placeable cameras
	std::vector<std::string> camera_names;
	std::vector<std::shared_ptr<Texture>> camera_logos;
	std::vector<std::shared_ptr<Camera>> camera_types;

	std::shared_ptr<Camera> fly_camera;
	std::shared_ptr<Camera> current_camera;

	std::vector<std::unique_ptr<Model>> models;
	Mesh view_plane;

	std::shared_ptr<Program> phong_shading_program;
	std::shared_ptr<Program> post_process_program;

	std::array<float, 4> clear_color{ 0.0f, 0.0f, 0.0f, 0.0f };

	std::shared_ptr<Renderpass> main_pass;
	std::shared_ptr<MSAA_Renderpass> msaa_main_pass;
	std::shared_ptr<Renderpass> postprocess_pass;

	glm::vec2 uv_offset = glm::vec2(0);
	glm::vec2 uv_scale  = glm::vec2(1);


	void glfw_init();
	void glew_init();
	void opengl_setup(); // sets opengl settings

	void load_models(const std::string& model_path);
	void load_programs();
	void setup_camera(float fovy, float near_plane, float far_plane);
	void setup_renderpasses();

	void setup_placeable_cameras(float near_plane, float far_plane);

	bool resize_window = false;
	void resize_resources(); // is called before each frame if glf_framebffer_size_callback sets resize to true

	bool recompile_shaders = false;
public:
	std::unique_ptr<Controller> controller;

	inline void should_resize(uint32_t new_res_x, uint32_t new_res_y);
};

inline void App::should_resize(uint32_t new_res_x, uint32_t new_res_y)
{
	res_x = new_res_x;
	res_y = new_res_y;
	resize_window = true;
}

void glfw_error_callback(int error, const char* description);
void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfm_mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);