#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "engine/Camera.h"
#include "engine/Renderpass.h"
#include "Exporter.h"

#include <memory>
#include <vector>

// handles user i/o from window using glfw

class Controller {
public:
	Controller(std::shared_ptr<Camera> fly_cam, std::vector<std::shared_ptr<Camera>> cam_types, double res_x, double res_y, const std::string& output_folder);
	void handle_keys(GLFWwindow* window, const std::vector<std::pair<unsigned int, Camera>>& placed_cameras);
	void handle_mouse(double xpos, double ypos);
	bool place_camera(GLFWwindow* window, std::shared_ptr<Renderpass> renderpass, float placement_distance, std::vector<std::pair<unsigned int, Camera>>& placed_cameras); // if enter pressed, goes to rotation mode at position determined by depth texture; returns true if need to resize

	void init_time();
	void update_time();

	inline double get_dt() const { return delta_time; };
private:
	std::shared_ptr<Camera> current_camera;

	float rot_strength = 0.001;
	float move_strength = 5.0;

	// previous mouse position
	double xlast;
	double ylast;

	// tracking delta time
	double current_time;
	double prev_time;
	double delta_time;

	// pause movement
	bool pause_pressed = false; // toggle can move
	bool can_move = true;

	// freeze virtual camera (important to test frustum culling) for culling while still moving
	bool virtual_freeze_pressed = false; // toggle virtual camera freeze
	bool virtual_camera_freeze = false;

	// switch to preview mode
	bool enter_pressed = false;
	bool preview_mode = false;

	// export camera list
	bool export_pressed = false;
	const std::string output_folder;

	// current camera used
	unsigned int current_camera_type;
	std::shared_ptr<Camera> fly_camera;
	std::vector<std::shared_ptr<Camera>> camera_types;
public:
	inline void set_move_strength(float strength) { move_strength = strength; };
	inline void set_rotation_strength(float strength) { rot_strength = strength; };
	inline bool can_move_round() { return can_move; };
	inline bool in_preview_mode() { return preview_mode; };

	inline bool set_camera_type(unsigned int camera_type); // returns true if needs to resize
	inline std::shared_ptr<Camera> get_current_camera() const { return current_camera; };
};

inline bool Controller::set_camera_type(unsigned int camera_type)
{
	bool change_camera = current_camera_type != camera_type && preview_mode;
	if (change_camera) {
		// if we switch camera during preview mode, update it to current camera values
		std::shared_ptr<Camera> camera = camera_types.at(camera_type);
		camera->set_pos(current_camera->get_pos());
		camera->set_dir(current_camera->get_dir());
		camera->set_virtual_camera_enabled(virtual_camera_freeze);

		current_camera = camera;
	}

	current_camera_type = camera_type;

	return change_camera;
}