#include "material.h"
#include <iostream>


void Material::Enable()
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
}

void Material::Disable()
{
    if (!shader) return;
    shader->Disable();
}
