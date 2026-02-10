#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "entity.h"
#include "camera.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
	
	this->e1 = new Entity(); //creem les entitats
	this->e2 = new Entity();
	this->e3 = new Entity();
	this->render_mode = 1; //inicialitzem el mode de render
	this->current_prop = 'V'; //inicialitzem la propietat a modificar de la camara
	Mesh* mesh1 = new Mesh();
	mesh1->LoadOBJ("meshes/lee.obj"); //carrega arxiu especificat
	Mesh* mesh2 = new Mesh();
	mesh2->LoadOBJ("meshes/anna.obj");
	Mesh* mesh3 = new Mesh();
	mesh3->LoadOBJ("meshes/cleo.obj"); 


	e1->mesh = mesh1; // He fet els atributs publics enlloc de fer servir getters
	e1->type = 0; //animació de translacio
	e1->model.MakeTranslationMatrix(-3,0,0); // matriu model mou en l eix z

	e2->mesh = mesh2; 
	e2->type = 1; //animacio de rotacio
	//e2->model.MakeTranslationMatrix(0, 0, 0); aixo no fa res si hi ha animacio pq update ho mou

	e3->mesh = mesh3;
	e3->type = 2;//animacio d'escalat
	// e3->model.MakeTranslationMatrix(5, 5, 3); aixo no fa res si hi ha animacio pq update ho mou

	//camera
	this->camera = new Camera();
	float fov_graus = 60.0f;
	float aspect = framebuffer.width / float(framebuffer.height);//aspect radio perq no es deformi la imatge
	this->camera->SetPerspective(fov_graus*DEG2RAD, aspect, 0.01f, 1000.0f); //inicialitzem matriu de persepctiva

	Vector3 eye(0, 15, 5); //coloquem l'eye lluny per poder veure els objecte
	Vector3 center(0, 15, 0);
	Vector3 up(0, 1, 0);
	this->camera->LookAt(eye, center, up);
	e1->texture = new Image();
	if (!e1->texture->LoadTGA("textures/lee_color_specular.tga", true)) { // El 'true' voltea la Y
		std::cout << "Error carregant textura de Lee" << std::endl;
	}
	e2->texture = new Image();
	if (!e2->texture->LoadTGA("textures/anna_normal.tga", true)) { // El 'true' voltea la Y
		std::cout << "Error carregant textura de anna" << std::endl;
	}
	e3->texture = new Image();
	if (!e3->texture->LoadTGA("textures/cleo_color_specular.tga", true)) { // El 'true' voltea la Y
		std::cout << "Error carregant textura de Cleo" << std::endl;
	}
}

// Render one frame
void Application::Render(void)
{
framebuffer.Fill(Color::BLACK);// neteja framebuffer
	
// Crear i netejar el Z-Buffer en cada frame
FloatImage zBuffer(framebuffer.width, framebuffer.height);
zBuffer.Fill(10000.0f);
// Render entities

if (render_mode == 1) {
	e1->Render(&framebuffer, camera, &zBuffer, show_texture, use_zbuffer, use_interpolation);
} 
else if (render_mode == 2) {
	e1->Render(&framebuffer, camera, &zBuffer, show_texture, use_zbuffer, use_interpolation);
	e2->Render(&framebuffer, camera, &zBuffer, show_texture, use_zbuffer, use_interpolation);
}

	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

e1->Update(seconds_elapsed);
e2->Update(seconds_elapsed);
e3->Update(seconds_elapsed);

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
		case SDLK_1: render_mode = 1; break;
		case SDLK_2: render_mode = 2; break;


		//propietats de la camara 
		case SDLK_n: current_prop = 'N'; std::cout << "Prop: NEAR" << std::endl; break;
		case SDLK_f: current_prop = 'F'; std::cout << "Prop: FAR" << std::endl; break;
		case SDLK_v: current_prop = 'V'; std::cout << "Prop: FOV" << std::endl; break;

		case SDLK_PLUS:
		case SDLK_KP_PLUS: // Tsi apretem mes
			if (current_prop == 'V') camera->fov = clamp(camera->fov + 5 * DEG2RAD, 1 * DEG2RAD, 175 * DEG2RAD);
			if (current_prop == 'N') camera->near_plane += 0.1f;
			if (current_prop == 'F') camera->far_plane += 1.0f;
			camera->UpdateProjectionMatrix();
			break;

		case SDLK_MINUS: //si apretem menys
		case SDLK_KP_MINUS:
			if (current_prop == 'V') camera->fov = clamp(camera->fov - 5 * DEG2RAD, 1 * DEG2RAD, 175 * DEG2RAD);
			if (current_prop == 'N') camera->near_plane = std::max(0.01f, camera->near_plane - 0.1f);
			if (current_prop == 'F') camera->far_plane = std::max(camera->near_plane + 1.0f, camera->far_plane - 1.0f);
			camera->UpdateProjectionMatrix();
			break;

		case SDLK_t:
			show_texture = !show_texture;
			// Toggle entre utilitzar textura de malla o color per vèrtex 
			std::cout << "Toggle: Mode Textura" << std::endl;
			break;
		case SDLK_z:
			use_zbuffer = !use_zbuffer;
			std::cout << "Toggle: Z-Buffer" << std::endl;
			break;
		case SDLK_c:
			use_interpolation = !use_interpolation;
			// Toggle entre UVs interpolades o color pla 
			std::cout << "Toggle: Interpolacio" << std::endl;
			break;
	}

}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	if(mouse_state &SDL_BUTTON_LMASK){//si apretem boto esquerre orbitem
		camera->Rotate(mouse_delta.x * 0.01f, Vector3(0, 1, 0)); //rotacio al voltant de l eix y
	//mouse_delta.x representa quan sha mogut horitzontalment desde el ultim frame
	//camera->rotate la cridem per aplicar el moviment com angle de rotacio
	//ector3(0,1,0) indica q la rotacio es fa sobre el eix y
		Vector3 right = camera->GetLocalVector(Vector3(1, 0, 0));
		camera->Rotate(mouse_delta.y * 0.01f, right);
	}
	if (mouse_state & SDL_BUTTON_RMASK) {//si apretem boto dret fem desplaçament
		Vector3 right = camera->GetLocalVector(Vector3(1, 0, 0));
		Vector3 up = camera->GetLocalVector(Vector3(0, 1, 0));
		camera->center = camera->center - right * mouse_delta.x * 0.01f;
		camera->center = camera->center + up * mouse_delta.y * 0.01f;
		camera->UpdateViewMatrix();
	}
}


void Application::OnWheel(SDL_MouseWheelEvent event) //per fer zoom a la camara amb la roda del ratoli
{
	float dy = event.preciseY; //captura moviment de la roda

	// ...
	Vector3 forward = camera->center - camera->eye;; //calcula el vector de direccio (o vector cap endavant de la camera) restant la posicio actual de la camera(eye) del punt q esta mirant (center)
	float dist = forward.Length();
	forward.Normalize();
	float new_dist = dist - dy * 0.5f;//ajustem el zoom sense perdre la direccio de la camara
	if (new_dist > 0.1f) { // Evitam pasar el centre
		camera->eye = camera->center - forward * new_dist;
		camera->UpdateViewMatrix();
	}
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}

void Application::SetWindowSize(int width, int height)
{
	glViewport(0, 0, width, height);
	this->window_width = width;
	this->window_height = height;
	this->framebuffer.Resize(width, height);

	if (this->camera) {//perque quan canviem tamany finestra la malla no es deformi
		float aspect = framebuffer.width / float(framebuffer.height);
		this->camera->SetAspectRatio(aspect);
		this->camera->UpdateProjectionMatrix();
	}
}