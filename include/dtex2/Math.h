#ifndef _DTEX_MATH_H_
#define _DTEX_MATH_H_

#include <stdint.h>

namespace dtex
{

#ifndef IS_4TIMES
	#define IS_4TIMES(x) ((x) % 4 == 0)
#endif // IS_4TIMES

#ifndef IS_POT
	#define IS_POT(x) ((x) > 0 && ((x) & ((x) -1)) == 0)
#endif // IS_POT

struct Rect 
{
	int16_t xmin, ymin;
	int16_t xmax, ymax;
};

}

#endif // _DTEX_MATH_H_