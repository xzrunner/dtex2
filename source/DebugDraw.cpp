#include "DebugDraw.h"
#include "RenderAPI.h"

namespace dtex
{

void DebugDraw::Draw(int tex_id, int pos)
{
	float xmin, xmax, ymin, ymax;
	if (pos == 0) {
		xmin = ymin = -1;
		xmax = ymax = 1;
	} else if (pos == 1) {
		xmin = 0;
		xmax = 1;
		ymin = 0;
		ymax = 1;
	} else if (pos == 2) {
		xmin = -1;
		xmax = -0;
		ymin = 0;
		ymax = 1;
	} else if (pos == 3) {
		xmin = -1;
		xmax = -0;
		ymin = -1;
		ymax = -0;
	} else if (pos == 4) {
		xmin = 0;
		xmax = 1;
		ymin = -1;
		ymax = -0;
	}

	float vertices[8], texcoords[8];
	vertices[0] = xmin; vertices[1] = ymin;
	vertices[2] = xmax; vertices[3] = ymin;
	vertices[4] = xmax; vertices[5] = ymax;
	vertices[6] = xmin; vertices[7] = ymax;
	texcoords[0] = 0; texcoords[1] = 0;
	texcoords[2] = 1; texcoords[3] = 0;
	texcoords[4] = 1; texcoords[5] = 1;
	texcoords[6] = 0; texcoords[7] = 1;

	RenderAPI::SetProgram();
	RenderAPI::DrawBegin();
	RenderAPI::Draw(vertices, texcoords, tex_id);
	RenderAPI::DrawEnd();
}

}