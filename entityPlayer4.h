#pragma once

#include "player.h"

class entityPlayer4 : public player
{
public:
    entityPlayer4();

    virtual void initPlayerForGame();
    virtual void spawnTransition();
};
