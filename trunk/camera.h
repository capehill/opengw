#ifndef CAMERA_H
#define CAMERA_H

#include "point3d.h"

class camera
{
public:
    camera();

    void center();
    void followPlayer();
    void run();

    float mCurrentZoom;
    float mTargetZoom;

    Point3d mCurrentPos;
    Point3d mTargetPos;
};

#endif // CAMERA_H
