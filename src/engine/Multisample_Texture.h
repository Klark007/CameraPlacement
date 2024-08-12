#pragma once

#include "interface/Texture_Interface.h"

// Multisample Texture

class Multisample_Texture : public Texture_Interface {
public:
	Multisample_Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int samples, int i_form);
private:
	int internal_format;
	unsigned int nr_samples;
public:
	// this is only sensible if our original data was a null pointer
	void resize(unsigned int res_x, unsigned int res_y) override;
	void resize(unsigned int res_x, unsigned int res_y, unsigned int res_z) override;
};