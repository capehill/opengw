#ifndef ENTITYGRUNT_H
#define ENTITYGRUNT_H

#include "entity.h"

class entityGrunt : public entity
{
public:
    entityGrunt();

    virtual void run();
    virtual void spawnTransition();

    float mAnimationIndex;
};

#endif // ENTITYGRUNT_H
