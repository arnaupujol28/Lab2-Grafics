uniform mat4 u_model;
uniform mat4 u_viewprojection;

//fs
varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec2 v_uv;

void main()
{
    v_uv = gl_MultiTexCoord0.xy; // agafem coord de tex

    vec4 world_position = u_model * vec4(gl_Vertex.xyz, 1.0);
    v_world_position = world_position.xyz;

    // Transformar la normal world
    v_world_normal = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

    // screen
    gl_Position = u_viewprojection * world_position;
}