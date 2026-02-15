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
	std::cout << "Initiating app..." << std::endl;
	quad_mesh = new Mesh();
	quad_mesh->CreateQuad();

	shader_ex1 = new Shader();
	shader_ex2->LoaD("shaders/quadd.vs", "shaders/quad.fs");


}

// Render one frame
void Application::Render(void)
{
	// ...
//if exercici3 current==1çççç
	shader_ex1->Enable();
	quad_mesh->Rendder();
	shader
		framebuffer.Render();

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