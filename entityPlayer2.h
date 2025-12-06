#pragma once

#include "player.h"

class entityPlayer2 : public player
{
public:
    entityPlayer2();

    virtual void initPlayerForGame();
    virtual void spawnTransition();
};
