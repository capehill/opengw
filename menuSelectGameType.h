#ifndef MENU_SELECT_GAME_TYPE_H
#define MENU_SELECT_GAME_TYPE_H


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

#endif // MENU_SELECT_GAME_TYPE_H
