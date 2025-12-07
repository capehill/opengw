#pragma once

#include "player.hpp"

class entityPlayer4 : public player
{
  public:
    entityPlayer4();

    virtual void initPlayerForGame();
    virtual void spawnTransition();
};
