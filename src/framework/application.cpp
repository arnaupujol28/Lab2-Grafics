#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "entity.h"
#include "camera.h"
#include "framework.h"

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
	this->fruits = nullptr;
	this->messi = nullptr;
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	//Crear recursos

	//quad mesh
	quad_mesh = new Mesh();
	quad_mesh->CreateQuad();
	//mesh 2.5
	mesh = new Mesh();
	mesh->LoadOBJ("meshes/lee.obj");

	//shader de formules, fruites i messi
	shader_ex1 = new Shader();
	shader_ex1->Load("shaders/quad.vs", "shaders/quad.fs");
	//shader del 2.5
	shader_ex2 = new Shader();
	shader_ex2->Load("shaders/raster.vs", "shaders/raster.fs");

	//carreguem la textura
	fruits = Texture::Get("images/fruits.png"); 
	messi = Texture::Get("images/messi.png");
	//carregar textura 2.5
	persona = Texture::Get("textures/lee_color_specular.tga");
	
	// posar tot el del 2.5 en una entity
	entity = new Entity();
	entity->mesh = mesh;
	entity->shader = shader_ex2;
	entity->texture = persona;
	entity->model.SetIdentity();
	entity->material.shader = shader_ex2; // assignar el shader del material
	entity->material.color_texture = persona; // assignar la textura del material
	entities.push_back(entity); // afegir entities al final del vector
	
	//llum ambient
	ambient_intensity = 0.1f;

	// crear llum
	sLight light;
	light.position = Vector3(10.0f, 20.0f, 20.0f); // a dalt a la dreta
	light.color = Vector3(1.0f, 1.0f, 1.0f);       // llum blanca
	lights.push_back(light);

	// preparar la camara per veure 3D
	camera = new Camera();
	camera->SetPerspective(60.0f, window_width / float(window_height), 0.001f, 100.0f);
	camera->LookAt(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));

	
}

// Render one frame
void Application::Render(void)
{
	//std::cout << control_tasca << std::endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //netegem pantalla i buffer
	// ...

	if (control_tasca == 1)
	{
		shader_ex1->Enable();

		shader_ex1->SetUniform1("u_tasca", 1);
		shader_ex1->SetUniform1("u_subtasca", (int)(control_subtasca - 'a'));
		shader_ex1->SetUniform1("u_time", this->time);

		quad_mesh->Render();

		shader_ex1->Disable();
	}
	else if (control_tasca == 2 && fruits != nullptr) {
		shader_ex1->Enable();
		shader_ex1->SetUniform1("u_tasca", control_tasca);
		shader_ex1->SetUniform1("u_subtasca", (int)(control_subtasca - 'a'));//passe, submtasca com un enter
		shader_ex1->SetUniform1("u_time", this->time);
		shader_ex1->SetTexture("u_texture", fruits);
		quad_mesh->Render();
		shader_ex1->Disable();
	}
	else if (control_tasca == 3 && messi != nullptr) {
		shader_ex1->Enable();
		shader_ex1->SetUniform1("u_tasca", control_tasca);
		shader_ex1->SetUniform1("u_subtasca", (int)(control_subtasca - 'a'));//passe, submtasca com un enter
		shader_ex1->SetUniform1("u_time", this->time);
		shader_ex1->SetTexture("u_texture", messi);
		quad_mesh->Render();
		shader_ex1->Disable();
	}
	else if (control_tasca == 4) {
		if (control_subtasca == 'a') {
			glDisable(GL_DEPTH_TEST);
			shader_ex1->Enable();
			shader_ex1->SetUniform1("u_tasca", 3);
			shader_ex1->SetUniform1("u_subtasca", 0);
			shader_ex1->SetUniform1("u_time", 0.0f);

			if (persona != nullptr) {
				shader_ex1->SetTexture("u_texture", persona);
			}
			quad_mesh->Render();
			shader_ex1->Disable();
		}
		else if (control_subtasca == 'b') {

			// Activam el z-buffer
			glEnable(GL_DEPTH_TEST);
			//configurem camara

			camera->SetPerspective(60.0f, window_width / float(window_height), 0.1f, 1000.0f);
			//omplim tuberia de dades. info de la camera
			uniform_data.viewprojection = camera->viewprojection_matrix;
			uniform_data.camera_position = camera->eye;
			uniform_data.ambient_light = Vector3(ambient_intensity, ambient_intensity, ambient_intensity);

			uniform_data.light = lights[0];
			entity->Render(uniform_data);

			glDisable(GL_DEPTH_TEST);
		}

	}
	

}

// Called after render
void Application::Update(float seconds_elapsed)
{
	this->time += seconds_elapsed;//fem q el temps avanci
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
		
		case SDLK_1: control_tasca = 1; break;
		case SDLK_2: control_tasca = 2; 
			control_subtasca = 'z';
			break;
		case SDLK_3: control_tasca = 3; 
			control_subtasca = 'z';
			break;
		case SDLK_4: control_tasca = 4; break;// canviar
		case SDLK_5: control_tasca = 5; break;
		case SDLK_6: control_tasca = 6; break;

			// Teclas de la 'a' a la 'f' para las subtareas 
		case SDLK_a: control_subtasca = 'a'; break;
		case SDLK_b: control_subtasca = 'b'; break;
		case SDLK_c: control_subtasca = 'c'; break;
		case SDLK_d: control_subtasca = 'd'; break;
		case SDLK_e: control_subtasca = 'e'; break;
		case SDLK_f: control_subtasca = 'f'; break;
		case SDLK_l:
			std::cout << "Canviant escenes del lab 4/lab 5.." << std::endl;
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