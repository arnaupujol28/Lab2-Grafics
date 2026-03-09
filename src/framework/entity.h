#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include "mesh.h"
#include "image.h"
#include "shader.h"
#include "material.h"
#include <vector>

class Camera;
class Mesh;
class Texture;
class Shader;
class Entity {
	
	public:
	Entity(); // signatura constructor
	void Render(sUniformData& uniformData, const std::vector<sLight>& scene_lights, int num_lights_to_render);
	void Update(float seconds_elapsed);


	//atributs
	Mesh* mesh; //conte el conjunt de vertex i de triangles q defineixen l forma del objecte
	Matrix44 model; //defineix la posició, orientació i tamany del objecte en el mon
	Texture* texture; // he canviat Image* per Texture*
	Shader* shader;
	Material material;
	int type; // hi ha tres tipus d'animacions
};
#endif 
// !ENTITY_H
