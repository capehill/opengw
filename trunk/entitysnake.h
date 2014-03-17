#ifndef ENTITYSNAKE_H
#define ENTITYSNAKE_H

#include "entity.h"

class entitySnakeSegment;

class entitySnake : public entity
{
public:
    entitySnake();

    virtual void runTransition();
    virtual void run();
    virtual void spawnTransition();
    virtual void spawn();
    virtual void destroyTransition();
    virtual void destroy();
    virtual void indicateTransition();

    virtual entity* hitTest(const Point3d& pos, float radius);

    virtual void draw();

private:
    Point3d mTarget;
    entitySnakeSegment* mSegments;

    void updateTarget();
};

#endif // ENTITYSNAKE_H
