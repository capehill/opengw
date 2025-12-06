#pragma once

#include "entity.h"

class entityWeaver : public entity
{
public:
    entityWeaver();

    virtual void run();
    virtual void spawnTransition();

    float mAnimationIndex;
};
