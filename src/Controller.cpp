#include "Controller.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include <iostream>

#include "engine/Helper.h"
#include "Exporter.h"

Controller::Controller(std::shared_ptr<Camera> fly_cam, std::vector<std::shared_ptr<Camera>> cam_types, double res_x, double res_y, const std::string& output_folder)
	: current_camera{ fly_cam }, 
	  fly_camera { fly_cam },
	  camera_types { cam_types }, 
	  xlast{res_x / 2}, 
	  ylast{res_y / 2},
	  output_folder { output_folder }
{

}

// initilaized seperately from object to have timer more precise for first frame
void Controller::init_time()
{
	current_time = glfwGetTime();
	prev_time = current_time - (1.0 / 60);
}

void Controller::update_time()
{
	current_time = glfwGetTime();
	delta_time = current_time - prev_time;
	prev_time = current_time;
}

void Controller::handle_keys(GLFWwindow* window, const std::vector<std::pair<unsigned int, Camera>>& placed_cameras)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		pause_pressed = true;
	} else if (pause_pressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		pause_pressed = false;
		if (can_move) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		can_move = !can_move;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		virtual_freeze_pressed = true;
	}
	else if (virtual_freeze_pressed && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		virtual_freeze_pressed = false;

		virtual_camera_freeze = !virtual_camera_freeze;

		current_camera->set_virtual_camera_enabled(virtual_camera_freeze);
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		export_pressed = true;
	}
	else if (export_pressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		export_cameras(placed_cameras, output_folder);
		export_pressed = false;
	}

	if (!can_move || preview_mode)
		return;

	glm::vec2 delta = glm::vec2(0);

	if (glfwGetKey(window, GLFW_KEY_W))
		delta.x += 1;
	if (glfwGetKey(window, GLFW_KEY_S))
		delta.x -= 1;
	if (glfwGetKey(window, GLFW_KEY_D))
		delta.y += 1;
	if (glfwGetKey(window, GLFW_KEY_A))
		delta.y -= 1;

	delta = (delta != glm::vec2(0)) ? glm::normalize(delta) : delta;

	glm::vec3 pos = current_camera->get_pos();
	glm::vec3 dir = current_camera->get_dir();

	pos += delta.x * move_strength * dir * (float) delta_time;
	glm::vec3 side = glm::cross(dir, current_camera->get_up()); // can assume both normalized
	pos += delta.y * move_strength * side * (float) delta_time;

	current_camera->set_pos(pos);
}

void Controller::handle_mouse(double xpos, double ypos)
{
	if (!can_move) {
		xlast = xpos;
		ylast = ypos;
		return;
	}

	double dx = xpos - xlast;
	double dy = ypos - ylast;

	if (dx != 0 || dy != 0) {
		current_camera->roll_yaw((current_camera->points_up() ? dx : -dx) * rot_strength);
		current_camera->roll_pitch(-dy * rot_strength);
	}

	xlast = xpos;
	ylast = ypos;
}

bool Controller::place_camera(GLFWwindow* window, std::shared_ptr<Renderpass> renderpass, float placement_distance, std::vector<std::pair<unsigned int, Camera>>& placed_cameras)
{
	if (!can_move)
		return false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && preview_mode) {
		preview_mode = false;
		current_camera = fly_camera;
		return true;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		enter_pressed = true;
	}
	else if (enter_pressed && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
		enter_pressed = false;

		if (!preview_mode) {
			float nl_depth = renderpass->sample_depth(0.5, 0.5);

			if (nl_depth > 1 - 1e-8) {
				return false;
			}

			float linear_depth = current_camera->linearize_depth(nl_depth);

			// set pos and flip direction (invert)
			glm::vec3 destination = current_camera->get_pos() + current_camera->get_dir() * (linear_depth - placement_distance);
			float pitch = -current_camera->get_pitch();
			float yaw = current_camera->get_yaw() + M_PI;

			// switch cameras
			current_camera = camera_types.at(current_camera_type);

			current_camera->set_virtual_camera_enabled(virtual_camera_freeze);
		
			current_camera->set_pos(destination);
			current_camera->set_yaw(yaw);
			current_camera->set_pitch(pitch);
		}
		else {
			placed_cameras.emplace_back(std::make_pair(current_camera_type, *current_camera));
			
			current_camera = fly_camera;
		}

		preview_mode = !preview_mode;
		return true;
	}

	return false;
}
