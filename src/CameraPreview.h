#pragma once

#include "engine/Frustum.h"
#include "engine/Billboard.h"

#include <string>

// Class containing all data for exporting and displaying placed cameras

class CameraPreview {
public:
	CameraPreview(unsigned int type, const std::string& name, const Camera& camera, const Frustum& frustum, const Billboard& billboard);
private:
	unsigned int camera_type;
	std::string name;
	Camera camera;
	Frustum frustum;
	Billboard icon;
public:
	inline void set_dir(glm::vec3 dir) { icon.set_dir(dir); };
	void draw_frustum(std::shared_ptr<Program> program) const;
	void draw_icon(std::shared_ptr<Program> program) const;

	inline Camera get_camera() const { return camera; };
	inline std::string get_name() const { return name; };
	inline unsigned int get_camera_type() const { return camera_type; };
};