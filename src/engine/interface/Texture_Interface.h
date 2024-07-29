#pragma once

#include <GL/glew.h>

#include "../Program.h"

#include <string>
#include <memory>

enum Texture_Filter
{
	linear = GL_LINEAR,
	nearest = GL_NEAREST,
	linear_mipmap = GL_LINEAR_MIPMAP_LINEAR,
	nearest_mipmap = GL_NEAREST_MIPMAP_NEAREST,
};
enum Texture_Wrap
{
	repeat = GL_REPEAT,
	clamp = GL_CLAMP_TO_EDGE,
};
Texture_Filter get_mipmap_filter(Texture_Filter f);

class Texture_Interface
{
public:
	Texture_Interface(const std::string& n, GLenum t, unsigned int res_x, unsigned int res_y);
	Texture_Interface(const std::string& n, GLenum t, unsigned int res_x, unsigned int res_y, unsigned int res_z);
	~Texture_Interface();
protected:
	unsigned int id;
	std::string name;

	unsigned int width;
	unsigned int height;
	unsigned int depth;

	GLenum type;
	int texture_unit=0;
public:
	void set_texture_unit(Program& program, int unit);
	void bind() const;
	void bind_texture_unit(Program& program, int unit);

	inline int get_id() const { return id; };
	inline unsigned int get_width() const { return width; };
	inline unsigned int get_height() const { return height; };
	inline unsigned int get_depth() const { return depth; };

	// assuming program is current program
	virtual void resize(unsigned int res_x, unsigned int res_y) = 0;
	virtual void resize(unsigned int res_x, unsigned int res_y, unsigned int res_z) = 0;
};

inline void Texture_Interface::set_texture_unit(Program& program, int unit)
{
	texture_unit = unit;
	program.set1i(name.c_str(), texture_unit);
}

inline void Texture_Interface::bind() const
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(type, id);
}

inline void Texture_Interface::bind_texture_unit(Program& program, int unit)
{
	set_texture_unit(program, unit);
	bind();
}