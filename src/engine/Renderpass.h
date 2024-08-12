#pragma once

#include "glm/glm.hpp"

#include "Texture.h"
#include "Program.h"

#include <memory>

// Renderpass encompassing clearing buffers, setting up textures for rendering into 

// TODO: support for depth only Renderpass
class Renderpass {
public:
	Renderpass(unsigned int res_x, unsigned int res_y, std::shared_ptr<Program> program, glm::vec4 clear_color, bool use_depth=true, float depth_clear_value = 1.0f, bool final_pass = false);
protected:
	unsigned int res_x;
	unsigned int res_y;

	bool use_depth;

	glm::vec4 color_clear_value;
	float depth_clear_value;

	std::shared_ptr<Program> program;
	std::shared_ptr<Texture> color_texture;
	std::shared_ptr<Texture> depth_texture;

	unsigned int framebuffer_id;
public:
	virtual void use();
	virtual void resize(unsigned int res_x, unsigned int res_y);
	
	float sample_depth(float pos_x, float pos_y) const;

	inline std::shared_ptr<Texture> get_color_texture() const { return color_texture; };
	inline std::shared_ptr<Texture> get_depth_texture() const { return depth_texture; };

};