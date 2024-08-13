#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "engine/Program.h"
#include "engine/Texture.h"
#include "CameraPreview.h"

#include <memory>
#include <vector>

// used to handle and setup gui

// variables that are set via gui
struct GuiOutput {
	unsigned int selected_camera_type = 0;

	float placement_distance = 0.1;


	float camera_near_plane;
	float camera_far_plane;
	float camera_rotation_speed = 0.001;
	float camera_movement_speed = 5.0;

	bool preview_frustums = true;
	bool preview_icon = true;
	// updating those will only change the next camera placed
	float preview_far_plane_scale = 0.25;
	float preview_icon_scale = 0.05;

	bool toggle_aabb_drawing = false;
	bool toggle_frustum_culling = false;
	bool toggle_fxaa = false;
	bool toggle_msaa = true;
};

class GUI {
public:
	GUI(GLFWwindow* window, std::shared_ptr<Program> program, std::vector<std::string> camera_names, std::vector<std::shared_ptr<Texture>> camera_logos, float near_plane, float far_plane);
	~GUI();
	void render(std::vector<std::shared_ptr<CameraPreview>>& placed_cameras);
private:
	std::shared_ptr<Program> program;

	std::vector<std::string> cameras_labels;
	std::vector<std::shared_ptr<Texture>> camera_logos;

	inline void imgui_frame_start();
	inline void imgui_frame_end();

	GuiOutput output;
public:
	inline GuiOutput get_gui_output() const { return output; };
};