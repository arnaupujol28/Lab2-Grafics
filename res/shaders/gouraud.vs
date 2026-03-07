uniform mat4 u_model;
uniform mat4 u_viewprojection;
uniform vec3 u_camera_position;
uniform vec3 u_ambient_light;

uniform vec3 u_light_position;
uniform vec3 u_light_color;

uniform vec4 u_material_color;
uniform float u_material_shininess;

varying vec2 v_uv;
varying vec3 v_color; //color iluminat final

void main()
{
	v_uv = gl_MultiTexCoord0.xy;//coordenades textura
	
	vec3 world_position = (u_model * gl_Vertex).xyz;
    	vec3 world_normal = (u_model * vec4(gl_Normal, 0.0)).xyz;

	//vectors iluminacio
	vec3 N = normalize(world_normal);
   	vec3 L = normalize(u_light_position - world_position);
    	vec3 V = normalize(u_camera_position - world_position);
    	vec3 R = normalize(reflect(-L, N));

	float NdotL = max(dot(N, L), 0.0); //dot product perq no hi hagin llums negatives
    	float RdotV = max(dot(R, V), 0.0);
	
//apliquem equació de phong
	vec3 base_color=vec3(0.7);
	vec3 ambient = base_color * u_ambient_light;
    	vec3 diffuse = base_color * u_light_color * NdotL;

	vec3 specular = vec3(0.0);
    	if (NdotL > 0.0) { //si la llum esta darrere lobjecte no te brillo especular
        	specular = vec3(1.0) * u_light_color * pow(RdotV, u_material_shininess);
    	}
	v_color = ambient + diffuse + specular;//color final del vertex
	gl_Position = u_viewprojection * vec4(world_position, 1.0);
}