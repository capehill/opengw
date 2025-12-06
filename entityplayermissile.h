#pragma once

#include "entity.h"

class entityPlayerMissile : public entity
{
public:
    entityPlayerMissile();

    virtual void run();
    virtual void spawnTransition();
    virtual void destroyTransition();

    virtual void draw();

    int mType; // TODO: entity class also has mType
    int mPlayerSource;
    float mVelocity { 0.0f };

private:
    Point3d mLastPos;

};
