#include "shader.h"
#include "texture.h"
#include "image.h"
#include "framework.h"


class Material {
	public:
	Shader* shader;
	Texture* color_texture;
	Texture* normal_texture;
	Color base;
	float shininess;

	 
	void Enable();
	void Disable();

	Material() {
	
	shader = nullptr;
	color_texture = nullptr;
	normal_texture = nullptr;
	base = Color::WHITE;
	shininess = 32.0;

	}
};

struct sLight {

	Vector3 pos;
	float color_intensity;
};

