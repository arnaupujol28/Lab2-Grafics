// This variables comes from the vertex shader
varying vec2 v_uv;
// They are baricentric interpolated by pixel according to the distance to every vertex
uniform sampler2D u_texture;


void main(){

	vec4 tex = texture2D(u_texture, v_uv);
	gl_FragColor = tex;
}
