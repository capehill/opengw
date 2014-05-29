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

    virtual entity* hitTest(const Point3d& pos, float radius);

    void setInitialSpeed(const Point3d& speed) { mInitialSpeed = speed; }

private:
    Point3d mVirtualPos;
    float mAnimationIndex;
    Point3d mInitialSpeed;

};

#endif // ENTITYTINYSPINNER_H
