// This variables comes from the vertex shader
varying vec2 v_uv; // ja interpolat per la gpu, no s'ha de programar
// They are baricentric interpolated by pixel according to the distance to every vertex
uniform sampler2D u_texture;


void main(){

	//vec2 uv = vec2(v_uv.x, 1.0 - v_uv.y);
	vec4 tex = texture2D(u_texture, v_uv);
	gl_FragColor = tex;
}
