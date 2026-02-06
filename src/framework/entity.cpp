#include "mesh.h"
#include "framework.h"
#include "entity.h"
#include "image.h"


Entity::Entity() // constructor, ho inicialitza buit i despres ja es posa el que calgui
{
    this->mesh = nullptr;
    this->model.SetIdentity();
    this->type = 0;
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c)
{
    
    const std::vector<Vector3>& vertexs = mesh->GetVertices(); //obtenim els vertexs de la malla

   
    for (int i = 0; i < vertexs.size(); i += 3) {//iterem sobre els vertexs de 3 en 3 perq es un triangle

        //vertexs en l'espai local
        Vector3 v1 = vertexs[i];
        Vector3 v2 = vertexs[i + 1];
        Vector3 v3 = vertexs[i + 2];

        //transformem de local a mon
        // per ferho multipliquem paer la matriu model de la entitat
        Vector3 w1 = model * v1;
        Vector3 w2 = model * v2;
        Vector3 w3 = model * v3;

        // Projectem a clip space
        //utilitzem funcion de la camara
        Vector3 p1 = camera->ProjectVector(w1); //passa el punt de world a clip space(en perspectiva o ortografic)
        Vector3 p2 = camera->ProjectVector(w2);
        Vector3 p3 = camera->ProjectVector(w3);

        // CLIPPING 
        // nomes renderitzem si esta dins del rang [-1,1}
        bool is_p1_inside = p1.x >= -1 && p1.x <= 1 && p1.y >= -1 && p1.y <= 1 && p1.z >= -1 && p1.z <= 1;
        bool is_p2_inside = p2.x >= -1 && p2.x <= 1 && p2.y >= -1 && p2.y <= 1 && p2.z >= -1 && p2.z <= 1;
        bool is_p3_inside = p3.x >= -1 && p3.x <= 1 && p3.y >= -1 && p3.y <= 1 && p3.z >= -1 && p3.z <= 1;
        if (!is_p1_inside && !is_p2_inside && !is_p3_inside) continue;// si els 3 estan fora no processem
        // clip space a screen space
        // Convertim el rang [-1, 1] a [0, width-1] i [0, height-1]
        float w = (float)framebuffer->width;
        float h = (float)framebuffer->height;

        // Formula: (coordenada + 1) * 0.5 * tamany
        Vector2 s1((p1.x + 1.0f) * 0.5f * (w - 1.0f), (p1.y + 1.0f) * 0.5f * (h - 1.0f));
        Vector2 s2((p2.x + 1.0f) * 0.5f * (w - 1.0f), (p2.y + 1.0f) * 0.5f * (h - 1.0f));
        Vector2 s3((p3.x + 1.0f) * 0.5f * (w - 1.0f), (p3.y + 1.0f) * 0.5f * (h - 1.0f));

        //Dibuixem les linies utilitzan el nostre algoritme DDA
        framebuffer->DrawLineDDA(s1.x, s1.y, s2.x, s2.y, c);
        framebuffer->DrawLineDDA(s2.x, s2.y, s3.x, s3.y, c);
        framebuffer->DrawLineDDA(s3.x, s3.y, s1.x, s1.y, c);
    }

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