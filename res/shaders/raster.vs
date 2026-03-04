// Global variables from the CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;

// Variables to pass to the fragment shader
varying vec2 v_uv;
//here create uniforms for all the data we need here




void main()
{	
	v_uv = gl_MultiTexCoord0.xy; // agafa coordenades de textura

	//projecta el vertex a la pantalla
	// Project the vertex using the model view projection matrix
	vec4 world_position = u_model * vec4(gl_Vertex.xyz, 1.0);
	gl_Position = u_viewprojection * world_position; //output of the vertex shader
}