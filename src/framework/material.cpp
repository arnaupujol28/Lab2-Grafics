#include "material.h"
#include <iostream>

//prova
void Material::Enable(const sUniformData& uniformData)
{
    if (!shader) {
        return;
    }

    shader->Enable();

    //els noms han de coincidir amb els del shader
    shader->SetUniform4("u_material_color", base.r, base.g, base.b, 1.0);
    shader->SetUniform1("u_material_shininess", shininess);

    if (color_texture)
        shader->SetTexture("u_color_texture", color_texture);

    if (normal_texture)
        shader->SetTexture("u_normal_texture", normal_texture);

    shader->SetMatrix44("u_model", uniformData.model);
    shader->SetMatrix44("u_viewprojection", uniformData.viewprojection);
    shader->SetVector3("u_camera_position", uniformData.camera_position);
    shader->SetVector3("u_ambient_light", uniformData.ambient_light);

    //enviem les dades de la llum
    shader->SetVector3("u_light_position", uniformData.light.position);
    shader->SetVector3("u_light_color", uniformData.light.color);
}

void Material::Disable()
{
    if (!shader) return;
    shader->Disable();
}
