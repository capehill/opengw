#pragma once

#include "entity.h"

class game;

class entitySpinner : public entity
{
public:
    entitySpinner(const game& constRef);

    virtual void run();
    virtual void destroyTransition();
    virtual void spawnTransition();

    virtual void hit(entity* aEntity);

    float mAnimationIndex;

private:
    Point3d mHitSpeed;
    const game& mGame;
};
