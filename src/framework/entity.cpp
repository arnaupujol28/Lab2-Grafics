#include "mesh.h"
#include "framework.h"
#include "entity.h"

Entity::Entity() // constructor, ho inicialitza buit i despres ja es posa el que calgui
{
    mesh = nullptr;
    model.SetIdentity();
}