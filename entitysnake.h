#pragma once

#include "entity.h"

#include <vector>

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
    std::vector<entitySnakeSegment> mSegments;

    void updateTarget();
};
