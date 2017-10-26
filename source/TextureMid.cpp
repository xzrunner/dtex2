#include "dtex2/TextureMid.h"
#include "dtex2/HardRes.h"
#include "dtex2/RenderAPI.h"
#include "dtex2/TextureFormat.h"
#include "dtex2/ResourceAPI.h"

#include <fault.h>

#include <stdint.h>
#include <string.h>
#include <assert.h>

namespace dtex
{

TextureMid::TextureMid(int width, int height, int bpp, bool init_pixels)
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
		m_id = InitPixels(width, height, bpp);
	}
}

TextureMid::~TextureMid()
{
}

int TextureMid::InitPixels(int width, int height, int bpp)
{
	size_t sz = width * height * bpp;
	uint8_t* empty_data = new uint8_t[sz];
	if (empty_data == nullptr) {
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

	memset(empty_data, 0, sz);

	assert(bpp == 4 || bpp == 2);
	int fmt = TEXTURE_RGBA8;
	if (bpp == 2) {
		fmt = TEXTURE_RGBA4;
	}

	int id = RenderAPI::CreateTexture(empty_data, width, height, fmt);
	delete[] empty_data;
	if (RenderAPI::OutOfMemory()) {
		fault("TextureMid CreateTexture fail.");
	}

	return id;
}

}