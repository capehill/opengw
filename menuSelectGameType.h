#pragma once

#include "defines.h"
#include "scene.h"

class menuSelectGameType
{
public:
    static void init(int player);
    static void run();
    static void draw();

    static int selection;
};
