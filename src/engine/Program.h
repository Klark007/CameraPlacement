#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <vector>
#include <memory>
#include <string>

#include <unordered_map>
#include <unordered_set>

#define LOG_LENGTH 512
#define UPDATE_UNIFORM_IF_CHANGED false

struct Uniform;

class Program
{
public:
	Program(std::vector<std::shared_ptr<Shader>>& shaders);
	Program(const std::vector<std::shared_ptr<Shader>>& shaders, std::unordered_map<std::string, Uniform> uniform_values, std::unordered_map<std::string, int> uniform_locations);
	~Program();

	void attach_shader(std::shared_ptr<Shader> shader);
	void link_program();

	// record current values of uniform using map
	void recompile();

	void use() const;
private:
	unsigned int program;
	char log[LOG_LENGTH];

	std::unordered_set<std::shared_ptr<Shader>> shaders;
	std::unordered_map<std::string, Uniform> uniform_values; // stores uniform values which are used if recompiling shaders
	std::unordered_map<std::string, int> uniform_locations;

	std::unordered_map<std::string, int> get_uniform_locations();
	int get_location(const std::string& name);
	inline void update_uniform(const std::string& name, Uniform u);
public:
	void get(unsigned int name, int* params) const;
	void throw_if_link_error();
	inline unsigned int get_id() const { return program; };

	void set1i(const std::string& name, int value);
	void set1f(const std::string& name, float value);

	void set_vec2f(const std::string& name, const glm::vec2& value);
	void set_vec2f(const std::string& name, float x, float y);

	void set_vec3f(const std::string& name, const glm::vec3& value);
	void set_vec3f(const std::string& name, float r, float g, float b);

	void set_mat4f(const std::string& name, const glm::mat4& value);

	inline std::vector<std::shared_ptr<Shader>> get_shaders() const;
	inline std::unordered_map<std::string, Uniform> get_uniform_values_map() const { return uniform_values; };
	inline std::unordered_map<std::string, int> get_uniform_locations_map() const { return uniform_locations; };
	static std::shared_ptr<Program> conditional_recompile(std::shared_ptr<Program> program);
};

inline std::vector<std::shared_ptr<Shader>> Program::get_shaders() const
{
	return std::vector<std::shared_ptr<Shader>>(shaders.begin(), shaders.end());
}

struct Uniform {
	enum
	{
		INT,
		FLOAT,
		VEC2,
		VEC3,
		MAT4
	} tag;
	union
	{
		int i;
		float f;
		glm::vec2 v2;
		glm::vec3 v3;
		glm::mat4 m4;
	} u;

	explicit Uniform(int x) { tag = INT; u.i = x; };
	explicit Uniform(float x) { tag = FLOAT; u.f = x; };
	Uniform(const glm::vec2& x) { tag = VEC2; u.v2 = x; };
	Uniform(const glm::vec3& x) { tag = VEC3; u.v3 = x; };
	Uniform(const glm::mat4& x) { tag = MAT4; u.m4 = x; };

	friend bool operator== (const Uniform& a, const Uniform& b) { return a.tag == b.tag && a.u.i == b.u.i; }
	friend bool operator!= (const Uniform& a, const Uniform& b) { return !(a == b); }

	inline void set(Program& p, const std::string& name) const {
		switch (tag)
		{
		case Uniform::INT:
			p.set1i(name, u.i);
			break;
		case Uniform::FLOAT:
			p.set1f(name, u.f);
			break;
		case Uniform::VEC2:
			p.set_vec2f(name, u.v2);
			break;
		case Uniform::VEC3:
			p.set_vec3f(name, u.v3);
			break;
		case Uniform::MAT4:
			p.set_mat4f(name, u.m4);
			break;
		}
	}
};

inline void Program::update_uniform(const std::string& name, Uniform u)
{
#if UPDATE_UNIFORM_IF_CHANGED
	if (uniform_values.find(name) != uniform_values.end() && uniform_values.at(name) == u) {
		return;
	}
#endif
	uniform_values.insert_or_assign(name, u);
}