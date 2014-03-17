#ifndef ENTITYWANDERER_H
#define ENTITYWANDERER_H

#include "entity.h"

class entityWanderer : public entity
{
public:
    entityWanderer();

    virtual void run();
    virtual void spawnTransition();

    float mCurrentHeading;
    float mAnimationIndex;
    bool mFlipped;
};

#endif // ENTITYWANDERER_H
