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
	this->control_tasca = 0;
	this->control_subtasca = ' ';
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
	shader_ex1->Load("shaders/quad.vs", "shaders/quad.fs");


}

// Render one frame
void Application::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //netegem pantalla i buffer
	// ...

	shader_ex1->Enable();
	shader_ex1->SetUniform1("u_tasca", control_tasca);
	shader_ex1->SetUniform1("u_subtasca", (int)(control_subtasca - 'a'));//passe, submtasca com un enter
	quad_mesh->Render();
	shader_ex1->Disable();


}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
		
		case SDLK_1: control_tasca = 1; break;
		case SDLK_2: control_tasca = 2; break;
		case SDLK_3: control_tasca = 3; break;
		case SDLK_4: control_tasca = 4; break;

			// Teclas de la 'a' a la 'f' para las subtareas 
		case SDLK_a: control_subtasca = 'a'; break;
		case SDLK_b: control_subtasca = 'b'; break;
		case SDLK_c: control_subtasca = 'c'; break;
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
	
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}