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

void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zbuffer, bool show_texture, bool use_zbuffer, bool use_interpolation) {
    {
    // btenim les dades de la malla (vèrtexs i Uvs
    const std::vector<Vector3>& vertices = mesh->GetVertices();
    const std::vector<Vector2>& uvs = mesh->GetUVs(); //

    //  Iterem sobre els vèrtexs de 3 en 3
        for (int i = 0; i < vertices.size(); i += 3) {

            // Vèrtexs en l'espai local
            Vector3 v1 = vertices[i];
            Vector3 v2 = vertices[i + 1];
            Vector3 v3 = vertices[i + 2];

            //Transformació: Local -> World -> Clip 
            Vector3 w1 = model * v1;
            Vector3 w2 = model * v2;
            Vector3 w3 = model * v3;

           // Projectem a clip space utilitzant la càmera
            Vector3 p1 = camera->ProjectVector(w1);
            Vector3 p2 = camera->ProjectVector(w2);
            Vector3 p3 = camera->ProjectVector(w3);

            //CLIPPING: Verifiquem si el triangle és visible 
            bool is_p1_inside = p1.x >= -1 && p1.x <= 1 && p1.y >= -1 && p1.y <= 1 && p1.z >= -1 && p1.z <= 1;
            bool is_p2_inside = p2.x >= -1 && p2.x <= 1 && p2.y >= -1 && p2.y <= 1 && p2.z >= -1 && p2.z <= 1;
            bool is_p3_inside = p3.x >= -1 && p3.x <= 1 && p3.y >= -1 && p3.y <= 1 && p3.z >= -1 && p3.z <= 1;

            if (!is_p1_inside && !is_p2_inside && !is_p3_inside) continue;

            
            float w = (float)framebuffer->width;
            float h = (float)framebuffer->height;

            // Passem de rang [-1, 1] a coordenades de píxel [0, width/height]
            Vector3 s1((p1.x + 1.0f) * 0.5f * (w - 1.0f), (p1.y + 1.0f) * 0.5f * (h - 1.0f), p1.z);
            Vector3 s2((p2.x + 1.0f) * 0.5f * (w - 1.0f), (p2.y + 1.0f) * 0.5f * (h - 1.0f), p2.z);
            Vector3 s3((p3.x + 1.0f) * 0.5f * (w - 1.0f), (p3.y + 1.0f) * 0.5f * (h - 1.0f), p3.z);

            // Dibuixem el triangle amb interpolació baricèntrica
            // Enviem el zbuffer, la textura de l'entitat i les coordenades UV de cada vèrtex
            if (!use_interpolation) {
                // Triangle amb color pla 
                framebuffer->DrawTriangle(s1.GetVector2(), s2.GetVector2(), s3.GetVector2(), Color::WHITE, true, Color::RED);
            }
            else {
                //Interpolació baricèntrica, Z-Buffer i Textures [cite: 275, 276, 277]
                framebuffer->DrawTriangleInterpolated(
                    s1, s2, s3,                                 // Posicions en pantalla (amb Z per a oclusions)
                    Color::RED, Color::GREEN, Color::BLUE,      // Colors de vèrtex per interpolar 
                    use_zbuffer ? zbuffer : nullptr,            // Buffer de profunditat 
                    show_texture ? this->texture : nullptr,     // Textura 
                    uvs[i], uvs[i + 1], uvs[i + 2]              // Coordenades UV per texturitzat 
                );
            }
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