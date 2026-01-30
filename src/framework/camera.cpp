#include "camera.h"

#include "main/includes.h"
#include <iostream>

Camera::Camera()
{
	view_matrix.SetIdentity();
	SetOrthographic(-1,1,1,-1,-10,10);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos)//agafa un punt en el mon i li aplica les matrius de vista i projeccio
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0); // passar a coords homogenies
	Vector4 result = viewprojection_matrix * pos4; // world to clip
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w; // dividir per w crea l'efecte de perspectiva
}

void Camera::Rotate(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.MakeRotationMatrix(angle, axis);
	Vector3 new_front = R * (center - eye);
	center = eye + new_front;
	UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}
/* construir la view matrix(mon--> camara, viewing transformation) la classe camara te els seus atributs, aqui es fan servir i es modifica la viewmatrix
* La view matrix es |R U -F T|
					|0 0  0 1| pero amb la manera de matrix44 canviem el vector T com a ultima fila i la coord homogenia com a ultima columna*/
void Camera::UpdateViewMatrix()// R eix x de la camara. Forward direccio on mira la cam(-z). U és l'up vector aproximadament ortogonal
{
	// Reset Matrix (Identity)
	view_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
	//SetExampleViewMatrix();

	view_matrix.SetIdentity();

	Vector3 F = center - eye;   // el vector forward(vector de la camara al punt on mira) es el centre(punt on mira) - eye(punt en el que esta) 
	F.Normalize();

	Vector3 R = F.Cross(up);    // right = F x U
	R.Normalize();

	Vector3 U = R.Cross(F);     // up corregit = R X U

	// Remember how to fill a Matrix4x4 (check framework slides)
	// Careful with the order of matrix multiplications, and be sure to use normalized vectors!
	
	// Columna 0 = R
	view_matrix.M[0][0] = R.x;
	view_matrix.M[0][1] = R.y;
	view_matrix.M[0][2] = R.z;
	view_matrix.M[0][3] = -R.Dot(eye);

	// Columna 1 = U
	view_matrix.M[1][0] = U.x;
	view_matrix.M[1][1] = U.y;
	view_matrix.M[1][2] = U.z;
	view_matrix.M[1][3] = -U.Dot(eye);

	// Columna 2 = -F
	view_matrix.M[2][0] = -F.x;
	view_matrix.M[2][1] = -F.y;
	view_matrix.M[2][2] = -F.z;
	view_matrix.M[2][3] = F.Dot(eye);

	// Columna 3 = (0,0,0,1)
	view_matrix.M[3][0] = 0.0f;
	view_matrix.M[3][1] = 0.0f;
	view_matrix.M[3][2] = 0.0f;
	view_matrix.M[3][3] = 1.0f;

	/* ho he fet directament enlloc de fer View matrix = R^-1 * T^-1 on R^-1 = Rx Ry Rz 0  i T^-1 = 1 0 0 -EYEx
	*																		   Ux Uy Uz 0           0 1 0 -EYEy
	*																		   Fx Fy Fz 0	        0 0 1 -EYEz
																			   0  0  0  1           0 0 0  1     */

	UpdateViewProjectionMatrix();
}

// Create a projection matrix(camara/view a view volume/clip space?)
void Camera::UpdateProjectionMatrix() 
{
	// Reset Matrix (Identity)
	projection_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
	//SetExampleProjectionMatrix();

	// Remember how to fill a Matrix4x4 (check framework slides)
	
	if (type == PERSPECTIVE) {
		
		float f = 1.0f / tanf(fov * 0.5f); // fov en radians
		float n = near_plane;
		float fa = far_plane;

		projection_matrix.Clear();//tot a 0 per claredat

		//pg 27 presentacio
		projection_matrix.M[0][0] = f / aspect; // aspect = w/h
		projection_matrix.M[1][1] = f;

		projection_matrix.M[2][2] = (fa + n) / (n - fa);
		projection_matrix.M[2][3] = (2.0f * fa * n) / (n - fa);

		projection_matrix.M[3][2] = -1.0f;
		projection_matrix.M[3][3] = 0.0f;
	}
	else if (type == ORTHOGRAPHIC) {
		// ...
		float l = left, r = right, t = top, b = bottom;
		float n = near_plane, fa = far_plane;

		projection_matrix.Clear();

		//pg 25
		projection_matrix.M[0][0] = 2.0f / (r - l);
		projection_matrix.M[1][1] = 2.0f / (t - b);
		projection_matrix.M[2][2] = -2.0f / (fa - n);
		projection_matrix.M[3][3] = 1.0f;

		projection_matrix.M[3][0] = -(r + l) / (r - l);
		projection_matrix.M[3][1] = -(t + b) / (t - b);
		projection_matrix.M[3][2] = -(fa + n) / (fa - n);
	} 

	UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = projection_matrix * view_matrix; // world to clip(view to clip * wolrd to view)
}

Matrix44 Camera::GetViewProjectionMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}
