#pragma once

#include "interface/Texture_Interface.h"

// Texture class for creating, loading textures

class Texture : public Texture_Interface
{
public:
	Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const float* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map = false);
	Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const unsigned char* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map = false);
	Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const void* data, int data_type, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map = false);
	
	Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const float* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap);
	Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const unsigned char* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap);
	Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const void* data, int data_type, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap);
private:
	// stored for resizing
	bool can_resize;
	int i_form;
	int form;
	int d_type;
public:
	// this is only sensible if our original data was a null pointer
	void resize(unsigned int res_x, unsigned int res_y) override;
	void resize(unsigned int res_x, unsigned int res_y, unsigned int res_z) override;

	inline GLenum get_format() const { return form; };
	inline GLenum get_data_type() const { return d_type; };
};

std::shared_ptr<Texture> create_texture_from_file(const std::string& n, const std::string& path, Texture_Filter filter = Texture_Filter::linear, Texture_Wrap wrap=Texture_Wrap::repeat, bool mip_map = true);