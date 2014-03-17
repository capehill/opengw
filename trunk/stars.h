#ifndef STARS_H
#define STARS_H

#include "point3d.h"

#define NUM_STARS 8000

class stars
{
public:
    stars();
    ~stars();

    void run();
    void draw();

    typedef struct
    {
        Point3d pos;
        float radius;
    }STAR;

    STAR* mStars;
};

#endif // STARS_H
