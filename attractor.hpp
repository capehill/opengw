#pragma once

#include "particle.hpp"
#include "point3d.hpp"
#include <vector>

class attractor
{
  public:
    attractor();

    void clearAll();

    Point3d evaluateParticle(particle::PARTICLE* point);

    typedef struct
    {
        Point3d pos;
        float strength;
        float radius;
        bool enabled;
        bool attractsParticles;
    } Attractor;

    attractor::Attractor* getAttractor();

    std::vector<Attractor> mAttractors;
    int mNumAttractors;
};
