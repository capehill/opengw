#ifndef ENTITYPLAYERMISSILE_H
#define ENTITYPLAYERMISSILE_H

#include "entity.h"

class entityPlayerMissile : public entity
{
public:
    entityPlayerMissile();

    virtual void run();
    virtual void spawnTransition();
    virtual void destroyTransition();

    virtual void draw();

    int mType;
    int mPlayerSource;
    float mVelocity;

private:
    Point3d mLastPos;

};

#endif // ENTITYPLAYERMISSILE_H
