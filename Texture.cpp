#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Texture::createTexture(IDirect3DDevice9* device, std::string file)
{
	path = file;

	std::wstring src = std::wstring(file.begin(), file.end());

	D3DXCreateTextureFromFile(device, src.c_str(), &txt);
}
