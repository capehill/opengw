#ifndef BOMB_H
#define BOMB_H

#include "Point3d.h"
#include "vector.h"

class bomb
{
public:

    typedef struct
    {
        Point3d pos;
        float radius;
        float thickness;
        float speed;
        int timeToLive;
        float fadeStep;
        vector::pen pen;
    }RING;

    bomb(void);
    ~bomb(void);

    void startBomb(Point3d pos, float radius, float thickness, float speed, int timeToLive, vector::pen pen);

    void run();
    void draw();

    bool isBombing();

    RING* mRings;
    int mNumRings;

};

#endif

