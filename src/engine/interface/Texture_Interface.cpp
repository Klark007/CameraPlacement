#include "Texture_Interface.h"

Texture_Interface::Texture_Interface(const std::string& n, GLenum t, unsigned int res_x, unsigned int res_y)
	: name{ n }, type{ t }, width { res_x }, height { res_y }
{
	glGenTextures(1, &id);
	glBindTexture(type, id);
}

Texture_Interface::Texture_Interface(const std::string& n, GLenum t, unsigned int res_x, unsigned int res_y, unsigned int res_z)
	: name{ n }, type{ t }, width{ res_x }, height{ res_y }, depth { res_z }
{
	glGenTextures(1, &id);
	glBindTexture(type, id);
}

Texture_Interface::~Texture_Interface()
{
	glDeleteTextures(1, &id);
}

Texture_Filter get_mipmap_filter(Texture_Filter f) {
	switch (f)
	{
	case Texture_Filter::linear:
		return Texture_Filter::linear_mipmap;
	case Texture_Filter::nearest:
		return Texture_Filter::nearest_mipmap;
	}
}