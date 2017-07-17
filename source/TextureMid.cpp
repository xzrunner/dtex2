#include "TextureMid.h"
#include "HardRes.h"
#include "RenderAPI.h"
#include "DTEX_TextureFormat.h"
#include "ResourceAPI.h"

#include <fault.h>

#include <stdint.h>
#include <string.h>

namespace dtex
{

TextureMid::TextureMid(int width, int height, bool init_pixels)
{
	const int max_edge = HardRes::GetMaxTexSize();
	if (width > max_edge) {
		width = max_edge;
	}
	if (height > max_edge) {
		height = max_edge;
	}
	SetSize(width, height);

	if (init_pixels) {
		m_id = InitPixels(width, height);
	}
}

TextureMid::~TextureMid()
{
}

int TextureMid::InitPixels(int width, int height)
{
	uint32_t* empty_data = new uint32_t[width*height];
	if (empty_data == NULL) {
		ResourceAPI::ErrorReload();
		return -1;
	}

// 	uint32_t col = 0x66ff0000;
// 	int ptr = 0;
// 	for (int i = 0; i < width; ++i) {
// 		for (int j = 0; j < height; ++j) {
// 			memcpy(&empty_data[ptr++], &col, 4);
// 		}
// 	}

	memset(empty_data, 0, width*height*4);

	int id = RenderAPI::CreateTexture(empty_data, width, height, TEXTURE_RGBA8);
	delete[] empty_data;
	if (RenderAPI::OutOfMemory()) {
		fault("TextureMid CreateTexture fail.");
	}

	return id;
}

}