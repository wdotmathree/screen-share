#pragma once
#ifndef _INCLUDE_H
#define _INCLUDE_H

#include <SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32

#else // __linux__

#include <immintrin.h>

#endif

#endif
