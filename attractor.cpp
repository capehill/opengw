#include "attractor.hpp"
#include "game.hpp"
#include "mathutils.hpp"
#include "settings.hpp"

attractor::attractor()
{
    // PERFORMANCE: If you set this too high things will start to slow down in grid::run()!!
    mAttractors.resize(settings::get().mAttractors);
}

attractor::Attractor* attractor::getAttractor()
{
    for (auto& a: mAttractors) {
        if (!a.enabled) {
            return &a;
        }
    }

    return nullptr;
}

void attractor::clearAll()
{
    for (auto& a: mAttractors) {
        a.enabled = false;
    }
}

Point3d attractor::evaluateParticle(particle::PARTICLE* p)
{
    Point3d speed(0.0f, 0.0f, 0.0f);

    for (const auto& a: mAttractors) {
        if (a.enabled && a.attractsParticles) {
            const Point3d& apoint = a.pos;

            const float angle = mathutils::calculate2dAngle(p->posStream[0], apoint);
            float distance = mathutils::calculate2dDistance(p->posStream[0], apoint);

            if (distance < a.radius) {
                distance = a.radius;
            }

            const float r = 1.0f / (distance * distance);

            // Add a slight curving vector to the gravity
            Point3d gravityVector(-r * a.strength * .5f, 0.0f, 0.0f);                  // .5
            Point3d g = mathutils::rotate2dPoint(gravityVector, angle + .25f); // .35 , .7

            speed.x += g.x;
            speed.y += g.y;
        }
    }

    return speed;
}
