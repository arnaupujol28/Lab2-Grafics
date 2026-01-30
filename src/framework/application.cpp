#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "entity.h"
#include "camera.h"
//hola
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
	e1->model.MakeTranslationMatrix(0,0,-5); // matriu model mou en l eix z

	e2->mesh = mesh2; 
	e2->model.MakeTranslationMatrix(0, 0, 0);

	e3->mesh = mesh2;
	e3->model.MakeTranslationMatrix(0, 0, 1);

	e1->type = 0; 
	e2->type = 1; 
	e3->type = 2; 

	//camera
	this->camera = new Camera();
	/*float aspect = framebuffer.width / float(framebuffer.height);
	this->camera->SetPerspective(60.0f, aspect, 0.01f, 1000.0f); No se si cal*/


}

// Render one frame
void Application::Render(void)
{
framebuffer.Clear(Color::BLACK);// neteja framebuffer
	
// Render entities

if (render_mode == 1) {
	e1->Render(&framebuffer, camera, Color::WHITE);// momes 1
} 
else if (render_mode == 2) {
	e1->Render(&framebuffer, camera, Color::WHITE);
	e2->Render(&framebuffer, camera, Color::WHITE);
	e3->Render(&framebuffer, camera, Color::WHITE);
{

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
			if (current_prop == 'V') camera->fov += 5 * DEG2RAD;
			if (current_prop == 'N') camera->near_plane += 0.1f;
			if (current_prop == 'F') camera->far_plane += 1.0f;
			camera->UpdateProjectionMatrix(); // Recalculem matriu de projeccio
			break;

		case SDLK_MINUS: //si apretem menys
		case SDLK_KP_MINUS:
			if (current_prop == 'V') camera->fov -= 5 * DEG2RAD;
			if (current_prop == 'N') camera->near_plane -= 0.1f;
			if (current_prop == 'F') camera->far_plane -= 1.0f;
			camera->UpdateProjectionMatrix(); // Recalculem matriu de projeccio
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
	if(mouse_state &SDLBUTTON_LMASK){//si apretem boto esquerre orbitem
		camera->Rotate(mouse_delta.x * 0.01f, Vector3(0, 1, 0)); //rotacio al voltant de l eix y
	//mouse_delta.x representa quan sha mogut horitzontalment desde el ultim frame
	//camera->rotate la cridem per aplicar el moviment com angle de rotacio
	//ector3(0,1,0) indica q la rotacio es fa sobre el eix y
	}
	if (mouse_State & SDL_BUTTON_RMASK) {//si apretem boto dret fem desplaçament
		camera->center.x -= mouse_delta.x * 0.01f;
		camera->center.y += mouse_delta.y * 0.01f;
		//quan canvime el center la camara apunta a un altre lloc de lescenari
		camera->UpdateViewMatrix();
	}
}


void Application::OnWheel(SDL_MouseWheelEvent event) //per fer zoom a la camara amb la roda del ratoli
{
	float dy = event.preciseY; //captura moviment de la roda

	// ...
	Vector3 forward = camera->center - camera->eye;; //calcula el vector de direccio (o vector cap endavant de la camera) restant la posicio actual de la camera(eye) del punt q esta mirant (center)
	camera->eye = camera->eye + forward * dy * 0.1f;//modifica la posicio fisica de la camera, si movem cap endavant dy es positiut ens apropem al objecte
	// si movem cap enrere dy es negatiu i ens allunyem de l objecte
	camera->UpdateViewMatrix(); //recalcula la matriu de vista
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}

