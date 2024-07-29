#include "Shader.h"

#include <GL/glew.h>
#include "Exception.h"

Shader::Shader(unsigned int shader_type)
{
	shader = glCreateShader(shader_type);
}

Shader::Shader(unsigned int shader_type, const std::string& path)
{
	shader = glCreateShader(shader_type);
	this->path = path;
	recompile();
}

Shader::~Shader()
{
	glDeleteShader(shader);
}

void Shader::add_source_from_file()
{
	// read file and at it to shader source
	std::ifstream file;
	file.open(path);

	if (!file.is_open()) {
		throw IOException("Shader file not found at: " + path, __FILE__, __LINE__);
	}

	std::stringstream source_stream;
	source_stream << file.rdbuf();

	std::string source_string = source_stream.str();
	const char* source_code = source_string.c_str();

	glShaderSource(shader, 1, &source_code, NULL);
	file.close();
	return;
}

void Shader::compile()
{
	glCompileShader(shader);
}

void Shader::recompile()
{
	add_source_from_file();
	compile();
	throw_if_compile_error();
}

void Shader::get(unsigned int name, int* params) const
{
	glGetShaderiv(shader, name, params);
}

void Shader::throw_if_compile_error()
{
	int status;
	get(GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderInfoLog(shader, LOG_LENGTH, NULL, log);
		throw ShaderException("Shader compilation failed: " + std::string(log), __FILE__, __LINE__);
	}
}