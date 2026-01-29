#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
class Image;
class Camera;
class Mesh;

class Entity {
	
	public:
	Entity(); // signatura constructor
	void Render(Image* framebuffer, Camera* camera, const Color& c);
	void Update(float seconds_elapsed);


	//atributs
	Mesh* mesh; //conte el conjunt de vertex i de triangles q defineixen l forma del objecte
	Matrix44 model; //defineix la posició, orientació i tamany del objecte en el mon
	int type; // hi ha tres tipus d'animacions
};

#endif // !ENTITY_H
