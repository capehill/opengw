#ifndef DEFINES_H
#define DEFINES_H

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// OpenGL support
#include <GL\\gl.h>
#include <GL\\glu.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

/*
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER 0x8D41
#endif
*/

#define PI (3.14159)
#define RADIAN (PI*2)


#endif // DEFINES_H
