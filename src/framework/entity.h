#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include "mesh.h"
#include "image.h"
class Image;
class Camera;
class Mesh;

class FloatImage;

class Entity {
	
	public:
	Entity(); // signatura constructor
	void Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer, bool show_texture, bool use_zbuffer, bool use_interpolation);
	void Update(float seconds_elapsed);


	//atributs
	Mesh* mesh; //conte el conjunt de vertex i de triangles q defineixen l forma del objecte
	Matrix44 model; //defineix la posició, orientació i tamany del objecte en el mon
	Image* texture;
	int type; // hi ha tres tipus d'animacions
};

#endif // !ENTITY_H
