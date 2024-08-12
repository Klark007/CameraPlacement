#pragma once

// Renderpass implementing MSAA

#include "Renderpass.h"
#include "Multisample_Texture.h"

class MSAA_Renderpass : public Renderpass {
public:
	MSAA_Renderpass(unsigned int res_x, unsigned int res_y, std::shared_ptr<Program> program, unsigned int nr_samples, glm::vec4 clear_color, bool use_depth = true, float depth_clear_value = 1.0f, bool final_pass = false);
protected:
	std::shared_ptr<Multisample_Texture> multisample_color_texture;
	std::shared_ptr<Multisample_Texture> multisample_depth_texture;
	unsigned int multisample_framebuffer_id;
public:
	void use() override;
	void resolve(); 
	void resize(unsigned int res_x, unsigned int res_y) override;
};