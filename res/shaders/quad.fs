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
		}else if (u_subtasca == 3){
    			vec2 uv = v_uv;          // IMPORTANT: declarar uv
    			float N = 16.0;          // nombre de divisions (graella)
    			// quantització a graella
    			vec2 uv_q = floor(uv * N) / (N - 1.0);
    			gl_FragColor = vec4(uv_q.x, uv_q.y, 0.0, 1.0);
			
		}else if (u_subtasca == 4){
    			vec2 uv = v_uv;

    			float N = 12.0;                  // nombre de quadrats per eix (ajusta-ho)
    			vec2 cell = floor(uv * N);       // (i,j) de la cel·la

    			// 0 o 1 alternant (escacs)
    			float checker = mod(cell.x + cell.y, 2.0);

    			// checker=0 -> negre, checker=1 -> blanc
    			gl_FragColor = vec4(vec3(checker), 1.0);
		}else if (u_subtasca == 5){
    			vec2 uv = v_uv;
	
    			float y0   = 0.55;
    			float amp  = 0.20;
    			float freq = 1.0;
    			float y_curve = y0 + amp * sin(6.2831853 * freq * uv.x);

    			// dos degradats fins a la corba
 
    			// per sota: g = uv.y 
    			float g_below = uv.y;

    			// per sobre: g = (1-uv.y)
    			float g_above = (1.0 - uv.y);

    			//Selecció
    			// s = 1 si uv.y >= y_curve, 0 altrament
    			float s = step(y_curve, uv.y);

    			// 4)si a dalt g_above, si a baix g_below
    			float g = mix(g_below, g_above, s);

    			g = clamp(g, 0.0, 1.0); //limitar valor
    			g = pow(g, 2.0); // contrast

    			gl_FragColor = vec4(0.0, g, 0.0, 1.0);
		}

		
	}
	
}
