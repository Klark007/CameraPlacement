#include "Renderpass.h"

#include <GL/glu.h>
#include <glm/gtc/type_ptr.hpp>

#include "Exception.h"

Renderpass::Renderpass(unsigned int res_x, unsigned int res_y, std::shared_ptr<Program> program, glm::vec4 clear_color, bool use_depth, float depth_clear_value, bool final_pass)
	: res_x{ res_x }, res_y{ res_y },
	program{ program },
	use_depth{ use_depth },
	color_clear_value{ clear_color },
	depth_clear_value{ depth_clear_value }
{
	if (final_pass) {
		framebuffer_id = 0;
	} else {
		glCreateFramebuffers(1, &framebuffer_id);

		color_texture = std::make_shared<Texture>(
			std::string("frame_color"), // uniform name in shader
			res_x, res_y,
			(float*)0,
			GL_RGB,
			GL_RGB,
			Texture_Filter::linear,
			Texture_Wrap::clamp
		);

		if (use_depth) {
			depth_texture = std::make_shared<Texture>(
				std::string("frame_depth"),
				res_x, res_y,
				(void*)0,
				GL_FLOAT,
				GL_DEPTH_COMPONENT,
				GL_DEPTH_COMPONENT,
				Texture_Filter::nearest,
				Texture_Wrap::clamp
			);
		}

		glNamedFramebufferTexture(framebuffer_id, GL_COLOR_ATTACHMENT0, color_texture->get_id(), 0);
		if (use_depth) {
			glNamedFramebufferTexture(framebuffer_id, GL_DEPTH_ATTACHMENT, depth_texture->get_id(), 0);
		}

		GLenum status = glCheckNamedFramebufferStatus(framebuffer_id, GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			throw RuntimeException("Renderpass creation failed with status " + std::string((const char*)gluErrorString(status)), __FILE__, __LINE__);
		}
	}
}

void Renderpass::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
	glViewport(0, 0, res_x, res_y);

	// clear the buffer
	glClearNamedFramebufferfv(framebuffer_id, GL_COLOR, 0, glm::value_ptr(color_clear_value));
	if (use_depth) {
		glClearNamedFramebufferfv(framebuffer_id, GL_DEPTH, 0, &depth_clear_value);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	program->use();
}

void Renderpass::resize(unsigned int res_x, unsigned int res_y)
{
	this->res_x = res_x;
	this->res_y = res_y;

	if (framebuffer_id != 0) {
		color_texture->resize(res_x, res_y);
		if (use_depth) {
			depth_texture->resize(res_x, res_y);
		}
	}
}

float Renderpass::sample_depth(float pos_x, float pos_y) const
{
	if (!use_depth) {
		throw RuntimeException("Tried to sample depth from renderpass without depth attachement", __FILE__, __LINE__);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	float depth;
	unsigned int screen_pos_x = std::roundf(pos_x * res_x);
	unsigned int screen_pos_y = std::roundf(pos_y * res_y);

	glReadPixels(screen_pos_x, screen_pos_y, 1, 1, depth_texture->get_format(), depth_texture->get_data_type(), &depth);

	return depth;
}
