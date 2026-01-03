#include "attractor.hpp"
#include "bomb.hpp"
#include "defines.hpp"
#include "enemies.hpp"
#include "game.hpp"

#include "SDL_opengl.h"

bomb::bomb(void)
{
    mRings.resize(20);
}

bomb::~bomb(void)
{
}

void bomb::run()
{
    for (auto& ring : mRings) {
        if (ring.timeToLive > 0) {
            ring.radius += ring.speed;
            ring.thickness += .03;
            --ring.timeToLive;

            if (ring.radius > 100) {
                ring.timeToLive = 0;
            }

            // Push the grid out
            attractor::Attractor* att = theGame->mAttractors->getAttractor();
            if (att) {
                att->strength = 200;
                att->radius = ring.radius;

                att->pos = ring.pos;
                att->enabled = true;
                att->attractsParticles = true;
            }

            // Look for any enemies within the blast radius and destroy them
            for (int j = 0; j < NUM_ENEMIES; j++) {
                if ((theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_INACTIVE) && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_INDICATING) && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_INDICATE_TRANSITION) && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_DESTROY_TRANSITION) && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_DESTROYED)) {
                    float distance = mathutils::calculate2dDistance(ring.pos, theGame->mEnemies->mEnemies[j]->getPos());
                    if ((distance > ring.radius - 10) && (distance < ring.radius)) {
                        // Destroy it
                        theGame->mEnemies->mEnemies[j]->hit(nullptr);
                    }
                }
            }
        }
    }
}

void bomb::draw()
{
    for (auto& ring : mRings) {
        if (ring.timeToLive > 0) {
            // Fade out
            ring.pen.a -= ring.fadeStep;
            if (ring.pen.a <= 0) {
                ring.pen.a = 0;
            } else if (ring.pen.a > 1) {
                ring.pen.a = 1;
            }

            glLineWidth(ring.pen.lineRadius);
            glColor4f(ring.pen.r, ring.pen.g, ring.pen.b, ring.pen.a);
            glBegin(GL_LINES);

            for (float a = 0; a < 360; a += .5) {
                float angle = mathutils::DegreesToRads(a);

                Point3d from(0, ring.radius, 0);
                from = mathutils::rotate2dPoint(from, angle);

                Point3d to(0, ring.radius + ring.thickness, 0);
                to = mathutils::rotate2dPoint(to, angle);

                from += ring.pos;
                to += ring.pos;

                glVertex3d(from.x, from.y, 0);
                glVertex3d(to.x, to.y, 0);
            }

            glEnd();
        }
    }
}

void bomb::startBomb(Point3d pos, float radius, float thickness, float speed, int timeToLive, vector::pen pen)
{
    RING* ring = nullptr;
    for (auto& r : mRings) {
        if (r.timeToLive <= 0) {
            ring = &r;
        }
    }

    if (ring) {
        ring->pos = pos;
        ring->radius = radius;
        ring->thickness = thickness;
        ring->speed = speed;
        ring->timeToLive = timeToLive;
        ring->fadeStep = 1.0f / ring->timeToLive;
        ring->pen = pen;

        Point3d angle(0, 0, 0);
        float spread = 2 * PI;
        int num = 100;
        int timeToLive = ring->timeToLive;
        pen.lineRadius = 5;
        pen.a = .3;
        theGame->mParticles->emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, false, false, 1, true);
    }
}

bool bomb::isBombing()
{
    for (const auto& ring : mRings) {
        if (ring.timeToLive > 0) {
            return true;
        }
    }
    return false;
}
