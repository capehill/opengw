#ifndef ENTITYGEOM_H
#define ENTITYGEOM_H

#include "entity.h"

class entityGeom : public entity
{
public:
    entityGeom();

    virtual void run();
    virtual void draw();
    virtual void spawnTransition();

    virtual void hit(entity* aEntity) {;} // yeah, thanks for letting me know
    entity* hitTest(const Point3d& pos, float radius) { return NULL; }

protected:
    int mLifetime;
};

#endif // ENTITYGEOM_H
