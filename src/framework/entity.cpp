#include "mesh.h"
#include "framework.h"
#include "entity.h"
#include "image.h"
#include <vector>
#include <algorithm>


Entity::Entity() // constructor, ho inicialitza buit i despres ja es posa el que calgui
{
    this->mesh = nullptr;
    this->model.SetIdentity();
    this->type = 0;
    this->texture = nullptr;
    this->shader = nullptr;
}

void Entity::Update(float seconds_elapsed) { // haura de ser cirdada per render de entity
    //s'encarrega de l'animacio modifica la matriu model
    static float time = 0.0f; // cal que sigui static per que aixi guarda el temps entre frames
    time += seconds_elapsed;

    Matrix44 T, R, S;
    model.SetIdentity(); //sempre resetejem abans de començar
    switch (type)
    {
    case 0: // nomes translacio
        T.MakeTranslationMatrix(sin(time) * 2.0f, 0.0f, -5.0f);
        model = T;
        break;

    case 1: // nomes rota sobre eix y
        T.MakeTranslationMatrix(0, 0, -5);
        R.MakeRotationMatrix(time, Vector3(0, 1, 0));
        model = T * R;
        break;

    case 2: // Nomes escalat
    {
        float s = 1.0f + 0.3f * sin(time);
        T.MakeTranslationMatrix(3, 0, -5);
        S.MakeScaleMatrix(s, s, s);
        model = T * S;
        break;
    }
    }

}

void Entity::Render(sUniformData& uniformData, const std::vector<sLight>& scene_lights, int num_lights_to_render)
{
    uniformData.model = this->model; // actualitzar model
    int max_lights = std::min(num_lights_to_render, (int)scene_lights.size());//calculem quantes llums utilitzarem
    if (max_lights <= 0) return;
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    uniformData.light = scene_lights[0];//primera llum
    material.Enable(uniformData); // activar material amb l'uniform data de application
    if (mesh != nullptr) { // pintar mesh
        mesh->Render();
    }
    if (max_lights > 1) {//si hi ha mes llums
        glDepthFunc(GL_LEQUAL); // Permet pintar a sobre del mateix píxel
        glEnable(GL_BLEND);     
        glBlendFunc(GL_ONE, GL_ONE);

        Vector3 original_ambient = uniformData.ambient_light;
        uniformData.ambient_light = Vector3(0.0f, 0.0f, 0.0f);

        for (int i = 1; i < max_lights; ++i) {

            // llum actual
            uniformData.light = scene_lights[i];

            // enviem dades i tornem a pintar
            material.Enable(uniformData);
            if (mesh != nullptr) {
                mesh->Render();
            }
        }
        uniformData.ambient_light = original_ambient;
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
    }
    material.Disable();
}
    

