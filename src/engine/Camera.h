 #pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Exception.h"

#define _USE_MATH_DEFINES
#include <math.h>

// Camera class which can generate view and projection matrices

class Camera
{
public:
	Camera() = default;
	Camera(glm::vec3 pos, glm::vec3 des, glm::vec3 up, unsigned int res_x, unsigned int res_y, float focx, float focy, float near_plane, float far_plane);
	Camera(glm::vec3 pos, glm::vec3 des, glm::vec3 up, unsigned int res_x, unsigned int res_y, float fov, float near_plane, float far_plane);

	inline void set_pos(glm::vec3 pos) { position = pos; }
	inline void look_at(glm::vec3 des) { set_dir(des - position); }
	
	inline void set_dir(glm::vec3 dir);

	inline void set_aspect_ratio(unsigned int r_x, unsigned int r_y);
	inline void set_virtual_camera_enabled(bool enabled);
	inline void roll_yaw(float d_yaw) { set_yaw(yaw + d_yaw); };
	inline void roll_pitch(float d_pitch) { set_pitch(pitch + d_pitch); };
	inline void set_yaw(float yaw) { this->yaw = yaw; };
	inline void set_pitch(float pitch);
	inline void set_near_plane(float near) { z_near = near; };
	inline void set_far_plane(float far) { z_far = far; };
private:
	glm::vec3 position;
	
	// glm::vec3 direction;
	float yaw;
	float pitch;

	unsigned int res_x;
	unsigned int res_y;
	float foc_x = -1; // set to invalid values if camera is constructed using fov and not focal length x and y
	float foc_y = -1;
	float fov;
	float z_near;
	float z_far;

	bool freeze_virtual_camera = false;
	glm::mat4 virtual_view_mat;

public:
	inline glm::mat4 generate_view_mat() const;
	inline glm::mat4 generate_view_mat_LHS() const;
	inline glm::mat4 generate_virtual_view_mat() const;
	inline glm::mat4 generate_projection_mat() const;
	inline glm::vec3 get_pos() const { return position; };
	inline glm::vec3 get_des() const { return position + get_dir(); };
	inline glm::vec3 get_dir() const;
	inline glm::vec3 get_up() const;
	inline float get_focal_len_x() const;
	inline float get_focal_len_y() const;

	inline float get_yaw() const { return yaw; };
	inline float get_pitch() const { return pitch; };
	inline float get_aspect_ratio() const {	return (float)res_x / res_y; };
	inline unsigned int get_resolution_x() const { return res_x; };
	inline unsigned int get_resolution_y() const { return res_y; };

	inline bool points_up() const  { return get_up().y > 0; };
	inline float linearize_depth(float d) const;
};

inline void Camera::set_dir(glm::vec3 dir)
{
	dir = glm::normalize(dir);

	yaw = atan2(dir.z, dir.x);
	pitch = asin(dir.y);
}

inline void Camera::set_aspect_ratio(unsigned int r_x, unsigned int r_y)
{
	res_x = r_x;
	res_y = r_y;
}

inline void Camera::set_virtual_camera_enabled(bool enabled)
{
	freeze_virtual_camera = enabled;
	if (freeze_virtual_camera) {
		virtual_view_mat = generate_view_mat();
	}
}

inline void Camera::set_pitch(float pitch)
{
	// limit pitch
	this->pitch = std::fmaxf(pitch, -M_PI_2 + 1e-5);
	this->pitch = std::fminf(this->pitch, M_PI_2 - 1e-5);
}

inline glm::mat4 Camera::generate_view_mat() const
{
	return glm::lookAt(position, position + get_dir(), get_up());
}

inline glm::mat4 Camera::generate_view_mat_LHS() const
{	
	// LHS lookAt for exporting
	return glm::lookAtLH(position, position + get_dir(), get_up());
}

inline glm::mat4 Camera::generate_virtual_view_mat() const
{
	if (freeze_virtual_camera) {
		return virtual_view_mat;
	}
	else {
		return generate_view_mat();
	}
}

inline glm::mat4 Camera::generate_projection_mat() const {
	return glm::perspective(fov, get_aspect_ratio(), z_near, z_far);
}

// calculate dir given yaw and pitch
inline glm::vec3 Camera::get_dir() const
{
	glm::vec3 dir;

	dir.x = (float)(cos(yaw) * cos(pitch));
	dir.y = (float)sin(pitch);
	dir.z = (float)(sin(yaw) * cos(pitch));

	return dir;
}

// calculate up given yaw and pitch (dir pitched up by PI/2)
inline glm::vec3 Camera::get_up() const
{
	glm::vec3 up;
	
	float up_pitch = pitch + M_PI_2;

	up.x = (float)(cos(yaw) * cos(up_pitch));
	up.y = (float)sin(up_pitch);
	up.z = (float)(sin(yaw) * cos(up_pitch));

	return up;
}

inline float Camera::get_focal_len_x() const
{
	if (foc_x == -1) {
		throw RuntimeException("Tried to access focal length x of camera that was initalized with field of view", __FILE__, __LINE__);
	}
	return foc_x;
}

inline float Camera::get_focal_len_y() const
{
	if (foc_y == -1) {
		throw RuntimeException("Tried to access focal length y of camera that was initalized with field of view", __FILE__, __LINE__);
	}
	return foc_y;
}

// compute linear depth
inline float Camera::linearize_depth(float d) const
{
	return z_near * z_far / (z_far + d * (z_near - z_far));
}

