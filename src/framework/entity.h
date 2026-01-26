#ifndef ENTITY_H
#define ENTITY_H

#include "mesh.h"
#include "framework.h"

#endif // !ENTITY_H


class Entity {
	
	public:
	Entity(); // signatura constructor


	//atributs
	Mesh* mesh; //conte el conjunt de vertex i de triangles q defineixen l forma del objecte
	Matrix44 model; //defineix la posició, orientació i tamany del objecte en el mon
};