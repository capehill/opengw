#ifndef PARTICLE_H
#define PARTICLE_H

#include "point3d.h"
#include "vector.h"
#include "mathutils.h"

//#define NUM_POS_STREAM_ITEMS 7
//#define NUM_POS_STREAM_ITEMS 4
#define NUM_POS_STREAM_ITEMS 5

class particle
{
public:
    typedef struct
    {
        Point3d posStream[NUM_POS_STREAM_ITEMS];

        float speedX;
        float speedY;
        float speedZ;

        vector::pen color;

        int timeToLive;

        float fadeStep;

        BOOL gravity;
        BOOL gridBound;
        float drag;

        BOOL hitGrid;

        BOOL glowPass;

    }PARTICLE;

    particle();
    ~particle();

    void draw();
    void run();

    void emitter(Point3d* position, Point3d* angle, float speed, float spread, int num, vector::pen* color, int timeToLive,
        BOOL gravity=TRUE, BOOL gridBound=TRUE, float drag=.93, BOOL glowPass=TRUE);

    void killAll();

    static PARTICLE* mParticles;
    static int mNumParticles;
    static int mIndex;

private:

    void assignParticle(Point3d* position,
                                  float aSpeedX, float aSpeedY, float aSpeedZ,
                                  int aTime, vector::pen* aColor, BOOL gravity, BOOL gridBound, float drag, BOOL glowPass);

};

#endif // PARTICLE_H
