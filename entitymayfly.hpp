#pragma once

#include "entity.hpp"

class entityMayfly : public entity
{
  public:
    entityMayfly();

    virtual void run();
    virtual void draw();

    int mFlipTimer;
    int mFlipDirection;

    Point3d mTarget;
};
