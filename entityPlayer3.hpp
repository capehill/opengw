#pragma once

#include "player.hpp"

class entityPlayer3 : public player
{
  public:
    entityPlayer3();

    virtual void initPlayerForGame();
    virtual void spawnTransition();
};
