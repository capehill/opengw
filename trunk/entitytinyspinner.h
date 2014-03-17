#ifndef ENTITYTINYSPINNER_H
#define ENTITYTINYSPINNER_H

#include "entitySpinner.h"

class entityTinySpinner : public entitySpinner
{
public:
    entityTinySpinner();

    virtual void run();
    virtual void spawnTransition();
    virtual void spawn();
    virtual void destroyTransition();
    virtual void destroy();

    virtual void draw();

private:
    Point3d mVirtualPos;
    float mAnimationIndex;

};

#endif // ENTITYTINYSPINNER_H
