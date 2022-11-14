#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

class Texture
{
	private:
		IDirect3DTexture9* txt = nullptr;
	public:
		struct ImageData
		{
			unsigned char* data;
			size_t size = 0;
		};

		std::string path;

		//int width, height, nrComponents;
		//int format;

		//ImageData imgData;

		void createTexture(IDirect3DDevice9* device, std::string file);

		IDirect3DTexture9* getTxt() const { return txt; }
};

