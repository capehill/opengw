#ifndef ENTITYSPINNER_H
#define ENTITYSPINNER_H

#include "entity.h"

class entitySpinner : public entity
{
public:
    entitySpinner();

    virtual void run();
    virtual void destroyTransition();
    virtual void spawnTransition();

    virtual void hit(entity* aEntity);

    float mAnimationIndex;

private:
    Point3d mHitSpeed;
};

#endif // ENTITYSPINNER_H
