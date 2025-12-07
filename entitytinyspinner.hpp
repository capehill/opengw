#pragma once

#include "entityspinner.hpp"

class game;

class entityTinySpinner : public entitySpinner
{
  public:
    entityTinySpinner(const game& gameRef);

    virtual void run();
    virtual void spawnTransition();
    virtual void spawn();
    virtual void destroyTransition();
    virtual void destroy();

    virtual void draw();

    virtual entity* hitTest(const Point3d& pos, float radius);

    void setInitialSpeed(const Point3d& speed) { mInitialSpeed = speed; }

  private:
    Point3d mVirtualPos { 0, 0, 0 };
    Point3d mInitialSpeed;

    const game& mGame;
};
