#pragma once

#include "point3d.h"

class game;

class camera
{
public:
    camera(const game& gameRef);

    void center();
    void followPlayer();
    void run();

    float mCurrentZoom;
    float mTargetZoom;

    Point3d mCurrentPos;
    Point3d mTargetPos;

    const game& mGame;
};
