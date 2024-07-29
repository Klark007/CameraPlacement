#pragma once

#include <string>
#include <fstream>
#include <sstream>

#define LOG_LENGTH 512


class Shader
{
public:
	Shader(unsigned int shader_type);
	Shader(unsigned int shader_type, const std::string& path);
	~Shader();

	void add_source_from_file();
	void compile();
	void recompile();

private:
	unsigned int shader;
	char log[LOG_LENGTH];
	std::string path;

public:
	void get(unsigned int name, int* params) const;
	void throw_if_compile_error();
	inline unsigned int get_id() const { return shader; };
};