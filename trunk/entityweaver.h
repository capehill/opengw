#ifndef ENTITYWEAVER_H
#define ENTITYWEAVER_H

#include "entity.h"

class entityWeaver : public entity
{
public:
    entityWeaver();

    virtual void run();
    virtual void spawnTransition();

    float mAnimationIndex;
};

#endif // ENTITYWEAVER_H
