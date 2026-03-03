#include "mesh.h"
#include "framework.h"
#include "entity.h"
#include "image.h"


Entity::Entity() // constructor, ho inicialitza buit i despres ja es posa el que calgui
{
    this->mesh = nullptr;
    this->model.SetIdentity();
    this->type = 0;
    this->texture = nullptr;
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

void Entity::Render(Camera* camera) 
{
    shader->Enable();
    Matrix44 model = this->model;
    Matrix44 viewprojection = camera->GetViewProjectionMatrix();
    
    shader->SetMatrix44("u_model", model);
    shader->SetMatrix44("u_viewprojection", viewprojection);
    shader->SetTexture("u_texture", texture);

    mesh->Render(GL_TRIANGLES);

    shader->Disable();
}
    

