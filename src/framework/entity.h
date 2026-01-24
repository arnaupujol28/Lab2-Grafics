#include "mesh.h"
#include "framework.h"

class Entity {
	
	public:
	Entity(Mesh mesh, Matrix44 model);
	~Entity();
	private:

	Mesh mesh;
	Matrix44 model;
};