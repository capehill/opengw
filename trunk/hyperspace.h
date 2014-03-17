#pragma once

#include "point3d.h"
#include "vector.h"

#define NUM_HS_STARS    2000

class hyperspace
{
public:
    hyperspace(void);
    ~hyperspace(void);

    void run();
    void draw();

    typedef struct
    {
        Point3d pos;
        Point3d lastPos;
        vector::pen color;
    }Star;

    float mTargetBrightness;
  
private:
    float mCurrentBrightness;

    Star mStars[NUM_HS_STARS];
};

