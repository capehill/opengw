#pragma once

#include "entity.hpp"

class game;

class entityGrunt : public entity
{
  public:
    entityGrunt(const game& gameRef);

    virtual void run();
    virtual void spawnTransition();

    float mAnimationIndex;

    const game& mGame;
};
