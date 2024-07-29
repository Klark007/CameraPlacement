#include "Multisample_Renderpass.h"

#include "Exception.h"

MSAA_Renderpass::MSAA_Renderpass(unsigned int res_x, unsigned int res_y, std::shared_ptr<Program> program, unsigned int nr_samples, glm::vec4 clear_color, bool use_depth, float depth_clear_value, bool final_pass)
	: Renderpass (res_x, res_y, program, clear_color, use_depth, depth_clear_value, final_pass)
{
	glCreateFramebuffers(1, &multisample_framebuffer_id);

	multisample_color_texture = std::make_shared<Multisample_Texture>(
		std::string("frame_color_msaa"),
		res_x, res_y, nr_samples,
		GL_RGB
	);

	if (use_depth) {
		multisample_depth_texture = std::make_shared<Multisample_Texture>(
			std::string("frame_depth_msaa"),
			res_x, res_y, nr_samples,
			GL_DEPTH24_STENCIL8
		);
	}

	glNamedFramebufferTexture(multisample_framebuffer_id, GL_COLOR_ATTACHMENT0, multisample_color_texture->get_id(), 0);
	if (use_depth) {
		glNamedFramebufferTexture(multisample_framebuffer_id, GL_DEPTH_ATTACHMENT, multisample_depth_texture->get_id(), 0);
	}

	GLenum status = glCheckNamedFramebufferStatus(multisample_framebuffer_id, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		const GLubyte* error = gluErrorString(status);
		if (error == 0) {
			throw RuntimeException("MSAA Renderpass creation failed with status " + status, __FILE__, __LINE__); // use website like https://james.darpinian.com/decoder
		}
		else {
			throw RuntimeException("MSAA Renderpass creation failed with status " + std::string((const char*)error), __FILE__, __LINE__);
		}
	}
	
}

void MSAA_Renderpass::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, multisample_framebuffer_id);
	glViewport(0, 0, res_x, res_y);

	// clear the buffer
	glClearNamedFramebufferfv(multisample_framebuffer_id, GL_COLOR, 0, glm::value_ptr(color_clear_value));
	if (use_depth) {
		glClearNamedFramebufferfv(multisample_framebuffer_id, GL_DEPTH, 0, &depth_clear_value);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	program->use();
}

void MSAA_Renderpass::resolve()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisample_framebuffer_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);

	glBlitFramebuffer(0, 0, res_x, res_y, 0, 0, res_x, res_y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	if (use_depth) {
		glBlitFramebuffer(0, 0, res_x, res_y, 0, 0, res_x, res_y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}
}

void MSAA_Renderpass::resize(unsigned int res_x, unsigned int res_y)
{
	this->res_x = res_x;
	this->res_y = res_y;

	multisample_color_texture->resize(res_x, res_y);
	if (use_depth) {
		multisample_depth_texture->resize(res_x, res_y);
	}
	
	if (framebuffer_id != 0) {
		color_texture->resize(res_x, res_y);
		if (use_depth) {
			depth_texture->resize(res_x, res_y);
		}
	}
}
