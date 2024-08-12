#include "Program.h"

#include <GL/glew.h>

#include "Exception.h"

#include <iostream>

Program::Program(std::vector<std::shared_ptr<Shader>>& shaders)
{
	program = glCreateProgram();

	this->shaders = std::unordered_set<std::shared_ptr<Shader>>();
	uniform_values = std::unordered_map<std::string, Uniform>();

	for (std::shared_ptr<Shader> s : shaders) {
		attach_shader(s);
	}

	link_program();
	throw_if_link_error();

	uniform_locations = get_uniform_locations();
}

Program::Program(const std::vector<std::shared_ptr<Shader>>& shaders, std::unordered_map<std::string, Uniform> uniform_values, std::unordered_map<std::string, int> uniform_locations)
{
	program = glCreateProgram();

	this->shaders = std::unordered_set<std::shared_ptr<Shader>>();
	this->uniform_values = uniform_values;
	this->uniform_locations = uniform_locations;

	for (std::shared_ptr<Shader> s : shaders) {
		attach_shader(s);
	}

	recompile();
}

Program::~Program()
{
	glDeleteProgram(program);
}

void Program::attach_shader(std::shared_ptr<Shader> shader)
{
	shaders.insert(shader);
	glAttachShader(program, shader->get_id());
}

void Program::link_program()
{
	glLinkProgram(program);
}

void Program::recompile()
{
	for (const std::shared_ptr<Shader>& shader : shaders) {
		shader->recompile();
	}

	link_program();
	throw_if_link_error();

	// restore uniforms
	std::unordered_map<std::string, int> new_uniform_locations = get_uniform_locations();
	std::unordered_map<std::string, int> old_uniform_locations = this->uniform_locations;
	this->uniform_locations = new_uniform_locations;

	use();
	for (const auto& uniform : uniform_values) {
		// only update if uniform was previously used
		if (old_uniform_locations.find(uniform.first) != old_uniform_locations.end()) {
			uniform.second.set(*this, uniform.first);
		}
	}
}

void Program::use() const
{
	glUseProgram(program);
}

std::unordered_map<std::string, int> Program::get_uniform_locations()
{
	std::unordered_map<std::string, int> uniform_locations = std::unordered_map<std::string, int>();

	GLint uniform_count = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

	if (uniform_count > 0) {
		GLint 	max_name_len = 0;
		GLsizei length = 0;
		GLsizei count = 0;
		GLenum 	type = GL_NONE;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

		char* name = new char[max_name_len];

		for (GLint i = 0; i < uniform_count; i++) {
			glGetActiveUniform(program, i, max_name_len, &length, &count, &type, name);

			uniform_locations.emplace(std::string(name), glGetUniformLocation(program, name));
		}

		delete[] name;
	}

	return uniform_locations;
}

int Program::get_location(const std::string& name)
{
	if (uniform_locations.find(name) == uniform_locations.end()) {
		return -1;
	}
	return uniform_locations[name];
}

void Program::get(unsigned int name, int* params) const
{
	glGetProgramiv(program, name, params);
}

void Program::throw_if_link_error()
{
	int status;
	get(GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramInfoLog(program, LOG_LENGTH, NULL, log);
		throw ShaderException("Program linking failed: " + std::string(log), __FILE__, __LINE__);
	}
}

std::shared_ptr<Program> Program::conditional_recompile(std::shared_ptr<Program> program)
{
	// try to recompile but only change if it compiles correctly
	try {
		return std::make_shared<Program>(program->get_shaders(), program->get_uniform_values_map(), program->get_uniform_locations_map());
	}
	catch (const CameraPlacementException& e) {
		std::cerr << e.what() << std::endl;
		return program;
	}
}


void Program::set1i(const std::string& name, int value)
{
	update_uniform(name, Uniform(value));
	glUniform1i(get_location(name), value);
}

void Program::set1f(const std::string& name, float value)
{
	update_uniform(name, Uniform(value));
	glUniform1f(get_location(name), value);
}

void Program::set_vec2f(const std::string& name, const glm::vec2& value)
{
	update_uniform(name, Uniform(value));
	glUniform2fv(get_location(name), 1, glm::value_ptr(value));
}

void Program::set_vec2f(const std::string& name, float x, float y)
{
	update_uniform(name, Uniform(glm::vec2(x, y)));
	glUniform2f(get_location(name), x, y);
}

void Program::set_vec3f(const std::string& name, const glm::vec3& value)
{
	update_uniform(name, Uniform(value));
	glUniform3fv(get_location(name), 1, glm::value_ptr(value));
}

void Program::set_vec3f(const std::string& name, float r, float g, float b)
{
	update_uniform(name, Uniform(glm::vec3(r, g, b)));
	glUniform3f(get_location(name), r, g, b);
}

void Program::set_mat4f(const std::string& name, const glm::mat4& value)
{
	update_uniform(name, Uniform(value));
	glUniformMatrix4fv(get_location(name), 1, GL_FALSE, glm::value_ptr(value));
}