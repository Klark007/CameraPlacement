#include "Texture.h"

#include "Exception.h"
#include "Helper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <filesystem>

// into single function
Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const float* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
	: Texture(n, res_x, res_y, (const void*)data, GL_FLOAT, internal_format, format, filter, wrap, mip_map)
{}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const unsigned char* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
	: Texture(n, res_x, res_y, (const void*)data, GL_UNSIGNED_BYTE, internal_format, format, filter, wrap, mip_map)
{}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const void* data, int data_type, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
	: Texture_Interface(n, GL_TEXTURE_2D, res_x, res_y)
{
	can_resize = (data == nullptr);

	i_form = internal_format;
	form = format;
	d_type = data_type;
	glTexImage2D(type, 0, i_form, res_x, res_y, 0, form, d_type, data);

	if (mip_map) {
		glGenerateMipmap(type);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, get_mipmap_filter(filter));
	}
	else {
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter);
	}
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const float* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap)
	: Texture(n, res_x, res_y, res_z, (const void*)data, GL_FLOAT, internal_format, format, filter, wrap)
{	
}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const unsigned char* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap)
	: Texture(n, res_x, res_y, res_z, (const void*)data, GL_UNSIGNED_BYTE, internal_format, format, filter, wrap)
{
}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const void* data, int data_type, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap)
	: Texture_Interface(n, GL_TEXTURE_3D, res_x, res_y, res_z)
{
	can_resize = (data == nullptr);

	i_form = internal_format;
	form = format;
	d_type = data_type;
	glTexImage3D(type, 0, i_form, res_x, res_y, res_z, 0, form, d_type, data);

	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, wrap);
}

// see https://gamedev.stackexchange.com/a/156707
void Texture::resize(unsigned int res_x, unsigned int res_y)
{
	if (!can_resize) {
		throw RuntimeException("Tried to resize texture" + name + " which was not set as resizeable", __FILE__, __LINE__);
	}

	width = res_x;
	height = res_y;

	glBindTexture(type, id);
	glTexImage2D(type, 0, i_form, width, height, 0, form, d_type, nullptr);
}

void Texture::resize(unsigned int res_x, unsigned int res_y, unsigned int res_z)
{
	if (!can_resize) {
		throw RuntimeException("Tried to resize texture" + name + " which was not set as resizeable", __FILE__, __LINE__);
	}

	width = res_x;
	height = res_y;
	depth = res_z;


	glBindTexture(type, id);
	glTexImage3D(type, 0, i_form, width, height, depth, 0, form, d_type, nullptr);
}

std::shared_ptr<Texture> create_texture_from_file(const std::string& n, const std::string& p, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
{
	std::string path = string_from_path(std::filesystem::current_path());
	path += "/";
	path += p;
	
	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (data) {
		// should choose internal format, potential issues with format not being one of the defined interal formats
		GLenum format;
		if (channels == 1)
			format = GL_RED;
		if (channels == 2)
			format = GL_RG;
		if (channels == 3)
			format = GL_RGB;
		if (channels == 4)
			format = GL_RGBA;

		std::shared_ptr<Texture> t = std::make_unique<Texture>(
			n,
			width,
			height,
			data,
			format,
			format,
			filter,
			wrap,
			mip_map
		);

		stbi_image_free(data);
		return t;
	}
	else
	{
		stbi_image_free(data);
		throw IOException("Texture not found: " + path, __FILE__, __LINE__);
	}
}
