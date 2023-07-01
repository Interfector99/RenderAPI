#include "matrix.h"

mat4_t mat4_identity(void)
{
	mat4_t identity =
	{
		{
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
		}
	};
	return identity;
}

mat4_t mat4_make_scale(float sx, float sy, float sz)
{
	mat4_t scale =
	{
		{
			{ sx,  0,  0,  0 },
			{  0, sy,  0,  0 },
			{  0,  0, sz,  0 },
			{  0,  0,  0,  1 }
		}
	};
	return scale;
}