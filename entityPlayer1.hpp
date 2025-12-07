#pragma once

#include "player.hpp"

class game;

class entityPlayer1 : public player
{
  public:
    entityPlayer1(const game& gameRef);

    virtual void initPlayerForGame();
    virtual void spawnTransition();

    const game& mGame;
};
