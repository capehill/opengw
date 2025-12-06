#pragma once

#include "player.h"

class entityPlayer3 : public player
{
public:
    entityPlayer3();

    virtual void initPlayerForGame();
    virtual void spawnTransition();
};
