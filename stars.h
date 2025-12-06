#pragma once

#include "point3d.h"

#define NUM_STARS 8000

class game;

class stars
{
public:
    stars(const game& gameRef);
    ~stars();

    void run();
    void draw();

    typedef struct
    {
        Point3d pos;
        float radius;
        float brightness;
		float twinkle;
    } STAR;

    STAR* mStars;
    const game& mGame;
};
