#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include <cmath> // std::round
#include <algorithm> // std::max


//cal posar Drawline, scanline, drawtriangle
//prova 2hbhvb
//joana
Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete[] pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixelUnsafe( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		if (pixels) delete[] pixels;
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {
		if (pixels) delete[] pixels;

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "--- File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;

		if (tgainfo->data != NULL)
			delete[] tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete[] pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	const char imageDescriptor = header[5];
	bool tgaFlipY = (imageDescriptor & 0x20) > 0; // bit 5 (0-7) -> true == origin on top
	bool tgaFlipX = (imageDescriptor & 0x10) > 0; // bit 4 (0-7) -> true == origin on right

	if (flip_y) {
		tgaFlipY = !tgaFlipY;
	}

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int offsetY = (tgaFlipY ? (height - 1 - y) : y) * width * bytesPerPixel;
			unsigned int offsetX = (tgaFlipX ? (width - 1 - x) : x) * bytesPerPixel;
			unsigned int pos = offsetY + offsetX;
			// Make sure we don't access out of memory
			if( pos + 2 < imageSize ) // assuming 1 bytes per channel
				SetPixelUnsafe(x, y, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	delete[] tgainfo->data;
	delete tgainfo;

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		std::cerr << "--- Failed to save file: " << fullPath.c_str() << std::endl;
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0; // image descriptor: origin in bottom-left

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	delete[] bytes;

	std::cout << "+++ File saved: " << fullPath.c_str() << std::endl;

	return true;
}

#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete[] pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

void Image::DrawLineDDA(float x0, float y0, float x1, float y1, const Color& c) {

	// resta
	float dx = x1 - x0;
	float dy = y1 - y0;

	//passos
	int steps = (int)std::max(std::fabs(dx), std::fabs(dy));

	// increment per pas
	float xInc = dx / (float)steps;
	float yInc = dy / (float)steps;

	float x = x0;
	float y = y0;

	for (int i = 0; i <= steps; ++i)
	{
		int xi = (int)std::round(x);
		int yi = (int)std::round(y);

		this->SetPixel(xi, yi, c);

		x += xInc;
		y += yInc;
	}
}

void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {//guarda les dades a la taula


	float diferenciax = (float)(x1 - x0);
	float diferenciay = (float)(y1 - y0);

	// pasos
	float pasos = std::max(std::abs(diferenciax), std::abs(diferenciay));

	// Si pasos es 0 no pintem
	if (pasos == 0) return;

	// Incremet
	float xInc = diferenciax / pasos;
	float yInc = diferenciay / pasos;

	float x = (float)x0;
	float y = (float)y0;

	// Recorrem la linea 
	for (int i = 0; i <= pasos; i++) {
		int currentY = (int)y;
		int currentX = (int)x;

		if (currentY >= 0 && currentY < (int)table.size()) { //nomes guatdem si la Y esta a dins de la imatge   table.size() es el mateix q la alçada de la imatge

			if (currentX < table[currentY].minx) { //el x q estic mirant ara es mes petit
				table[currentY].minx = currentX;
			}

			if (currentX > table[currentY].maxx) { //el x q miro es mes gran
				table[currentY].maxx = currentX;
			}
		}

		x += xInc;
		y += yInc;
	}
}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {
	// const Vector2& p0, const Vector2& p1, const Vector2& p2 son els 3 vertex del triangle
	//pose em & per passar el punt per refèrencia per que sigui mes rapid i no fem cap copia
	//borderColor el color de la bora
	//isFilled: true si volem pintar interior, false en cas contrari
	// el color de linterior es fillColor

	if (isFilled == true) { //si hem de pintar el triangle per dins fem

		//creem taula
		std::vector<Cell> table(height);

		//omplim la taula dientli on son les vores
		ScanLineDDA((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, table);
		ScanLineDDA((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, table);
		ScanLineDDA((int)p2.x, (int)p2.y, (int)p0.x, (int)p0.y, table);

		for (int y = 0; y < height; y++) {

			//si en aquesta fila hi ha bores
			if (table[y].minx <= table[y].maxx) {

				//evitem errors per sortinos de la pantalla
				int startX = std::max(0, table[y].minx);
				int endX = std::min((int)width - 1, table[y].maxx);

				//bucle per pintar la linia
				for (int x = startX; x <= endX; x++) {
					SetPixel(x, y, fillColor);
				}
			}
		}
	}
	//dibuixem bores
	DrawLineDDA((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, borderColor); //dibuixem aresta que va de p0 a p1
	DrawLineDDA((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, borderColor);
	DrawLineDDA((int)p2.x, (int)p2.y, (int)p0.x, (int)p0.y, borderColor);
}

// La he d'acabar
void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2) {

	// cal passar a 2d

	Vector2 A = Vector2(p0.x, p0.y);
	Vector2 B = Vector2(p1.x, p1.y);
	Vector2 C = Vector2(p2.x, p2.y);

	// Preparar per recorrer

	int minx = floor(std::min(A.x,B.x, C.x));
	int maxx = ceil(std::min(A.x, B.x, C.x));
	int miny = floor(std::min(A.y, B.y, C.y));
	int maxy = ceil(std::min(A.y, B.y, C.y));

	//Area

	float Area = (B-A).Perpdot(C-A); // calculat fent servir p0(A) com a origen
	// Perpdot fa el producte esclar de dos vectors 2d, el resultat nomes torna el component z de (0,0,z) que en valor absolut es el modul
	//suposem Area CCW
	if(Area == 0) return; // triangle sense area!

	//Recorrer

	for (int i = minx; i < maxx; ++i) {
		for (int j = miny; j < maxx; ++j) {

		Vector2 P = Vector2(i+0.5, j+0.5); // centre de cada pixel

		//basat en el dibuix de les diapositives posant sempre com a centre P
		float A0 = (B-P).Perpdot((C-P));
		float A1 = (C-P).Perpdot(A-P);
		float A2 = (A-P).Perpdot(B-P);

		double alpha = A0 / Area;
		double beta = A1/ Area;
		double gamma = A2/ Area;

		bool inside = (alpha >= 0 && beta >= 0 && gamma >= 0) || (alpha <= 0 && beta <= 0 && gamma <= 0);

		if (inside == false) continue; // saltar al seguent pixel

		//Interpolar color
		float r = alpha * c0.r + beta * c1.r + gamma * c2.r;
		float g = alpha * c0.g + beta * c1.g + gamma * c2.g;
		float b = alpha * c0.b + beta * c1.b + gamma * c2.b;

		Color out;
		out.r = (unsigned char)clamp(r, 0.0f, 255.0f);
		out.g = (unsigned char)clamp(g, 0.0f, 255.0f);
		out.b = (unsigned char)clamp(b, 0.0f, 255.0f);

		SetPixel(i, j, out);

		}
	}


}

