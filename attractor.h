#ifndef ATTRACTOR_H
#define ATTRACTOR_H

#include "point3d.h"
#include "particle.h"

class attractor
{
public:
    attractor();
    ~attractor();

    void clearAll();

    Point3d evaluateParticle(particle::PARTICLE* point);

    typedef struct
    {
        Point3d pos;
        float strength;
        float zStrength;
        float radius;
        bool enabled;
        bool attractsParticles;
    }Attractor;

    attractor::Attractor* getAttractor();

    Attractor* mAttractors;
    int mNumAttractors;

};

#endif // ATTRACTOR_H