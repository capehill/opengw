#include "defines.h"
#include "attractor.h"
#include "mathutils.h"

attractor::attractor()
{
    mNumAttractors = 50; // PERFORMANCE: If you set this too high things will start to slow down in grid::run()!!

    mAttractors = new Attractor[mNumAttractors];
    if (mAttractors)
    {
        clearAll();
    }
}

attractor::~attractor()
{
    if (mAttractors)
        delete mAttractors;
}

attractor::Attractor* attractor::getAttractor()
{
    for (int i=0; i<mNumAttractors; i++)
    {
        if (!mAttractors[i].enabled)
        {
            return &mAttractors[i];
        }
    }

    return NULL;
}

void attractor::clearAll()
{
    for (int i=0; i<mNumAttractors; i++)
    {
        mAttractors[i].enabled = false;
    }
}

Point3d attractor::evaluateParticle(particle::PARTICLE* p)
{
    Point3d speed(0,0,0);

    for (int i=0; i<mNumAttractors; i++)
    {
        Attractor att = mAttractors[i];
        if (att.enabled && att.attractsParticles)
        {
            Point3d apoint = att.pos;

            float angle = mathutils::calculate2dAngle(p->posStream[0], apoint);
            float distance = mathutils::calculate2dDistance(p->posStream[0], apoint);

            float strength = att.strength;

            if (distance < att.radius)
            {
                distance = att.radius;
            }

            float r = 1.0/(distance*distance);

            // Add a slight curving vector to the gravity
            Point3d gravityVector(-r * strength * .5, 0, 0); // .5
            Point3d g = mathutils::rotate2dPoint(gravityVector, angle+.35); // .7

            speed.x += g.x;
            speed.y += g.y;
        }
    }

    return speed;
}


