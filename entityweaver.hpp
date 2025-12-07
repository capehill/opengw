#pragma once

#include "entity.hpp"

class entityWeaver : public entity
{
  public:
    entityWeaver();

    virtual void run();
    virtual void spawnTransition();

    float mAnimationIndex;
};
