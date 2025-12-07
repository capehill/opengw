#pragma once

#include "entity.hpp"

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
