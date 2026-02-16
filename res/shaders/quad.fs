varying vec2 v_uv;
uniform int u_tasca;
uniform int u_subtasca;
void main()
{	gl_FragColor=vec4(0.0,0.0,0.0,1.0); //sino entra al if pinta negre
	
	if (u_tasca== 1) {
		if(u_subtasca==0){ //subtasca a
			vec4 blau= vec4(0.0,0.0,1.0,1.0);
			vec4 vermell=vec4(1.0,0.0,0.0,1.0);
			gl_FragColor = mix(blau,vermell,v_uv.x);

		}else if(u_subtasca==1){
			float d=distance(v_uv,vec2(0.5,0.5));//xalculem la distancia de cada pixel al centre
			vec4 negre=vec4(0.0,0.0,0.0,1.0); 
			vec4 blanc=vec4(1.0, 1.0, 1.0, 1.0);
			gl_FragColor = mix(negre,blanc,d);
		}else if(u_subtasca==2){
			float barraVertical=abs(sin(v_uv.x * 15.0)); 
            		float barraHoritzontal=abs(sin(v_uv.y * 15.0));
			barraVertical=pow(barraVertical,5.0);
			barraHoritzontal=pow(barraHoritzontal,5.0);
			vec4 vermell = vec4(barraVertical, 0.0, 0.0, 1.0);
            		vec4 blau= vec4(0.0, 0.0, barraHoritzontal, 1.0);
            		gl_FragColor = vec4(barraVertical,0.0,barraHoritzontal,1.0);
		}
	}
	
}
