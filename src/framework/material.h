#pragma once
#include "shader.h"
#include "texture.h"
#include "image.h"
#include "framework.h"

struct sLight {//estructura de llum
	Vector3 position;
	Vector3 color; 
};
struct sUniformData {//tuberia de dades el que viatja de la cpu a la gpu
	Matrix44 model;
	Matrix44 viewprojection;
	Vector3 camera_position;
	Vector3 ambient_light;
	sLight light;
};
class Material {
	public:
	Shader* shader;
	Texture* color_texture;
	Texture* normal_texture;
	Color base;
	float shininess;

	Material() {

		shader = nullptr;
		color_texture = nullptr;
		normal_texture = nullptr;
		base = Color::WHITE;
		shininess = 32.0;

	}
	 
	void Enable(const sUniformData& uniformData);
	void Disable();

	
};

