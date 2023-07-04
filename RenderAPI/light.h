#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

typedef struct
{
	vec3_t direction;
} light_t;

extern light_t light;

#endif // LIGHT_H