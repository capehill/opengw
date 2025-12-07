#pragma once

#include "point3d.hpp"
#include "vector.hpp"

#include <vector>

#define NUM_POS_STREAM_ITEMS 6

class particle
{
  public:
    struct PARTICLE
    {
        Point3d posStream[NUM_POS_STREAM_ITEMS];

        float speedX;
        float speedY;
        float speedZ;

        vector::pen color;

        int timeToLive;
        float fadeStep;
        float drag;

        bool gravity;
        bool gridBound;
        bool hitGrid;
        bool glowPass;
    };

    particle();
    ~particle();

    void draw();
    void run();

    void emitter(Point3d* position, Point3d* angle, float speed, float spread, int num, vector::pen* color, int timeToLive,
                 bool gravity = true, bool gridBound = true, float drag = .93, bool glowPass = true);

    void killAll();

    static std::vector<PARTICLE> mParticles;
    static int mNumParticles;
    static int mIndex;

  private:
    void assignParticle(Point3d* position,
                        const Point3d& speedVector,
                        int aTime, vector::pen* aColor, bool gravity, bool gridBound, float drag, bool glowPass);
};
