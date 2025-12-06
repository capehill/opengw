#pragma once

#include "entity.h"

class entityWanderer : public entity
{
public:
    entityWanderer();

    virtual void run();
    virtual void spawnTransition();

    float mCurrentHeading { 0.0f };
    float mAnimationIndex;
    bool mFlipped;
};
