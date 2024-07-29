#include "Multisample_Texture.h"

#include "Exception.h"

Multisample_Texture::Multisample_Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int samples, int i_form)
	: Texture_Interface(n, GL_TEXTURE_2D_MULTISAMPLE, res_x, res_y)
{
	nr_samples = samples;
	internal_format = i_form;
	glTexImage2DMultisample(type, nr_samples, internal_format, res_x, res_y, GL_TRUE);
}

void Multisample_Texture::resize(unsigned int res_x, unsigned int res_y)
{
	glBindTexture(type, id);
	
	width = res_x;
	height = res_y;

	glTexImage2DMultisample(type, nr_samples, internal_format, width, height, GL_TRUE);
}

void Multisample_Texture::resize(unsigned int res_x, unsigned int res_y, unsigned int res_z)
{
	throw NotImplementedException("Tried to resize a 3d multisampled texture which are not yet implemented", __FILE__, __LINE__);
}
