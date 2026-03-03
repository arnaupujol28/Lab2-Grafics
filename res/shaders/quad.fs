varying vec2 v_uv;
uniform int u_tasca;
uniform int u_subtasca;
uniform sampler2D u_texture;
uniform float u_time;
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
    			float N = 16.0;          // nombre de divisions (graella)
    			// quantització a graella
    			vec2 uv_q = floor(v_uv * N) / (N - 1.0);
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
    			// s = 1 estem sobre la corba , 0 altrament
    			float s = step(y_curve, uv.y);

    			//si a dalt g_above, si a baix g_below
    			float g = mix(g_below, g_above, s);

			// per que s'assembli més a l'exemple
    			g = clamp(g, 0.0, 1.0); //limitar valor
    			g = pow(g, 2.0); // contrast

    			gl_FragColor = vec4(0.0, g, 0.0, 1.0);
		}

		
	}
	if(u_tasca==2){
		vec4 texColor=texture2D(u_texture,v_uv);

		if(u_subtasca==0){ //quan apretem a. escala de grisos r+g+b / 3
			float gray=(texColor.r+texColor.g+texColor.b)/3.0;
			gl_FragColor = vec4(vec3(gray), 1.0);
		
		}else if(u_subtasca==1){//quan apretem b, negatiu 1 - rgb
			gl_FragColor = vec4(1.0 - texColor.rgb, 1.0); //restem 1 als canals RGB per poder obtenir el color oposat
		
		}else if(u_subtasca==2){//quan apretem c, groc = (1,1,0) passem la intensitat del pixel en quantitat de groc(vermell i verd)
			float gray=(texColor.r+texColor.g+texColor.b)/3.0;
			gl_FragColor = vec4(gray, gray, 0.0, 1.0);
			

		}else if(u_subtasca==3){//quan apretem d
			float gray=(texColor.r+texColor.g+texColor.b)/3.0;
			float s = step(0.5, gray); // si 0.5 > gray  , estem mes aprop del negre s = 0.0
			gl_FragColor = vec4(s,s,s,1.0);

		}else if(u_subtasca==4){//quan apretem e
			vec2 c = vec2(0.5, 0.5);
			float d = distance(v_uv, c);
			vec3 black = vec3(0.0, 0.0, 0.0);
			gl_FragColor = vec4(mix(texColor.rgb, black, d),1.0);
			
		
		}else if(u_subtasca==5){//quan apretem f, box blur 7x7 fa mitjana de la caixa 7x7 de la qual es el centre
			vec2 texelSize=vec2(1.0/1280.0,1.0/720.0);//perq sabem que la nostra imatge te dimension 1280x720 calculem quan medeix exacatement un pixel
			vec4 sum=vec4(0.0);
			for (int x = -3; x <= 3; x++) {
            			for (int y = -3; y <= 3; y++) {
                			vec2 offset = vec2(float(x), float(y)) * texelSize;
                			sum += texture2D(u_texture, v_uv + offset);
            			}
        		}
			gl_FragColor=sum/49.0;// dividim la suma entre el numero de mostres	
		}
		else{
			gl_FragColor = texColor;	
		}	

	}
	if(u_tasca==3){
		if(u_subtasca==0){//quan apreto a
			float angle = u_time * 2.0; //multipliquem per 2 perq giri més rapid
			float s = sin(angle);
			float c = cos(angle);
			
			vec2 uv = v_uv - vec2(0.5, 0.5);//movem el punt origen al centre de la textura
			//apliquem formila trigonometrica per girar
			vec2 uv_rotada = vec2(
				uv.x * c - uv.y * s,
				uv.x * s + uv.y * c
			) +vec2(0.5);
			// aquest if cal per fer-ho més elegant pero si s'elimina i es descomenta lo d'abaix tambe funciona
			if (uv_rotada.x < 0.0 || uv_rotada.x > 1.0 || uv_rotada.y < 0.0 || uv_rotada.y > 1.0)
    				gl_FragColor = vec4(1.0);     
			else
    			gl_FragColor = texture2D(u_texture, uv_rotada);
			//uv_rotada = uv_rotada + vec2(0.5, 0.5);//tornem a posar la imatge a la seva posició original
			//gl_FragColor = texture2D(u_texture, uv_rotada);//dibuicem la textura amb les noves coordenades

		}else if(u_subtasca==1){//quan apreto b
			float n = 20.0 + 15.0* sin(u_time);
			vec2 uv = floor(v_uv*n)/n;
			vec4 texColor=texture2D(u_texture,uv);
			gl_FragColor = texColor;				
 
		
		}else{
			gl_FragColor = texture2D(u_texture, v_uv);
		}
	}
}
