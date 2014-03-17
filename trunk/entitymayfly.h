#ifndef ENTITY_MAYFLY_H
#define ENTITY_MAYFLY_H

#include "entity.h"

class entityMayfly :
    public entity
{
public:
    entityMayfly();

    virtual void run();

    int mFlipTimer;
    int mFlipDirection;

    Point3d mTarget;
};

#endif

