#include "Material.h"

Material::Material(aiColor4t<float> diffuse, aiColor4t<float> specular, aiColor4t<float> ambient, aiColor4t<float> emissive, float shininess)
	: diffuse { diffuse },
	  specular { specular },
	  ambient { ambient },
	  emissive { emissive },
	  shininess { shininess }
{
}