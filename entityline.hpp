#pragma once

#include "entity.hpp"

class entityLine : public entity
{
  public:
    entityLine(void);

    virtual void run();
    virtual void spawnTransition();

    void addEndpoints(const Point3d& from, const Point3d& to);

    int mTimeToLive;
};
