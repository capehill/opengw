#pragma once

#include "defines.hpp"
#include "scene.hpp"

class menuSelectGameType
{
  public:
    static void init(int player);
    static void run();
    static void draw();

    static int selection;
};
