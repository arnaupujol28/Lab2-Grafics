varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec2 v_uv;

uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;

uniform vec3 u_light_position;
uniform vec3 u_light_color;

uniform vec3 u_material_ka;
uniform vec3 u_material_kd;
uniform vec3 u_material_ks;
uniform float u_material_shininess;

uniform sampler2D u_color_texture;
uniform sampler2D u_normal_texture;

uniform int u_use_color_texture;
uniform int u_use_specular_texture;
uniform int u_use_normal_texture;

uniform mat4 u_model;

void main()
{
    // normal per defecte
    vec3 N = normalize(v_world_normal);

    // si la normal map està activada, la fem servir
    if (u_use_normal_texture == 1)
    {
        vec3 normal_tex = texture2D(u_normal_texture, v_uv).rgb;

        // passar de [0,1] a [-1,1]
        normal_tex = normal_tex * 2.0 - 1.0;

        // passar de local space a world space
        N = normalize((u_model * vec4(normal_tex, 0.0)).xyz);
    }

    vec3 L = normalize(u_light_position - v_world_position);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 R = reflect(-L, N);

    // valors per defecte del material
    vec3 Ka = u_material_ka;
    vec3 Kd = u_material_kd;
    vec3 Ks = u_material_ks;

    // mostreig de la textura de color
    vec4 tex_color = texture2D(u_color_texture, v_uv);

    // si la textura de color està activada,
    // Ka i Kd passen a ser el color de la textura
    if (u_use_color_texture == 1)
    {
        Ka = tex_color.rgb;
        Kd = tex_color.rgb;
    }

    // si l'especular texture està activada,
    // Ks passa a ser l'alpha de la textura
    if (u_use_specular_texture == 1)
    {
        Ks = vec3(tex_color.a);
    }

    // ambient
    vec3 ambient = Ka * u_ambient_light;

    // diffuse
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * u_light_color * NdotL;

    // specular
    float spec = 0.0;
    if (NdotL > 0.0)
        spec = pow(max(dot(V, R), 0.0), u_material_shininess);

    vec3 specular = Ks * u_light_color * spec;

    // color final
    vec3 final_color = ambient + diffuse + specular;

    gl_FragColor = vec4(final_color, 1.0);
}