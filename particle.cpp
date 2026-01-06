#include "attractor.hpp"
#include "enemies.hpp"
#include "entityblackhole.hpp"
#include "game.hpp"
#include "grid.hpp"
#include "mathutils.hpp"
#include "particle.hpp"
#include "profiler.hpp"
#include "settings.hpp"
#include "scene.hpp"

#include <atomic>
#include <cstdio>
//#include <mutex>

#include "SDL_opengl.h"

static SDL_Thread* mRunThread = nullptr;
static std::atomic_bool mRunFlag { false };
static std::atomic_bool quitFlag { false };

std::vector<particle::PARTICLE> particle::mParticles;
std::size_t particle::mIndex = 0;

// static std::mutex m;

static int runThread(void* /*ptr*/)
{
    printf("Particle thread running\n");

    profiler prof("particle");

    while (!quitFlag) {
        while (!mRunFlag && !quitFlag) {
            SDL_Delay(1);
        }
        mRunFlag = false;

        prof.start();

        // TODO: fix data races
        // std::unique_lock<std::mutex> lock(m);

        const auto blackHoleStart = theGame->mEnemies->getBlackHoleStart();
        const auto blackHoleEnd = blackHoleStart + numEnemyBlackHole;

        for (auto& p: particle::mParticles) {
            if (p.timeToLive > 0) {
                // This particle is active

                if (--p.timeToLive < 0) {
                    // This particle died
                    p.timeToLive = 0;
                } else {
                    // Evaluate against attractors
                    if (p.gravity) {
                        const Point3d speed = theGame->mAttractors->evaluateParticle(&p);
                        p.speedX += speed.x;
                        p.speedY += speed.y;
                    }

                    // Evaluate against black holes
                    for (int j = blackHoleStart; j < blackHoleEnd; j++) {
                        auto blackHole = static_cast<entityBlackHole*>(theGame->mEnemies->mEnemies[j]);
                        if (blackHole->getState() == entity::ENTITY_STATE_RUNNING && blackHole->mActivated) {
                            if (mathutils::calculate2dDistance(p.posStream[0], blackHole->getPos()) < blackHole->getRadius() * 1.01f) {
                                // kill this particle
                                p.timeToLive *= .7f;
                                continue;
                            }
                        }
                    }

                    // Add drag
                    p.speedX *= p.drag;
                    p.speedY *= p.drag;

                    Point3d speedClamp(p.speedX, p.speedY, 0.0f);
                    speedClamp = mathutils::clamp2dVector(speedClamp, 2);
                    p.speedX = speedClamp.x;
                    p.speedY = speedClamp.y;

                    // Move the particle
                    p.posStream[0].x += p.speedX;
                    p.posStream[0].y += p.speedY;
                    p.posStream[0].z = 0.0f; // TODO: Point2d

                    if (p.gridBound) {
                        // Keep it within the grid

                        Point3d hitPoint;
                        Point3d speed(p.speedX, p.speedY);
                        if (theGame->mGrid->hitTest(p.posStream[0], 0, &hitPoint, &speed)) {
                            p.posStream[0] = hitPoint;

                            p.speedX = speed.x;
                            p.speedY = speed.y;
                        }
                    }

                    // Shift the position stream
                    for (int i = NUM_POS_STREAM_ITEMS - 2; i >= 0; i--) {
                        p.posStream[i + 1] = p.posStream[i];
                    }
                }
            }
        }

        prof.stop();
    }

    printf("Particle thread exiting\n");

    return 0;
}

particle::particle()
{
    mIndex = 0;

    // PERFORMANCE: The larger this number is, the larger the performance hit!
    mParticles.resize(settings::get().mParticles);

    for (auto& p: mParticles) {
        p.timeToLive = 0;
    }

    // Thread stuff
    mRunThread = SDL_CreateThread(runThread, "particle", nullptr);
    if (!mRunThread) {
        printf("Couldn't create particle run thread: %s\n", SDL_GetError());
    }
}

particle::~particle()
{
    quitFlag = true;

    int status = 0;
    SDL_WaitThread(mRunThread, &status);

    printf("Particle thread exited with status %d\n", status);

    for (auto& p: mParticles) {
        p.timeToLive = 0;
    }
}

void particle::emitter(Point3d* position, Point3d* angle, float speed, float spread, int num, vector::pen* color, int timeToLive,
                       bool gravity, bool gridBound, float drag, bool glowPass)
{

    // Emit a number of random thrust particles from the nozzle
    for (int p = 0; p < num; p++) {
        Point3d speedVertex, speedVector;

        speedVertex.x = 0.0f;
        speedVertex.y = speed * mathutils::frandFrom0To1();
        speedVertex.z = 0.0f;

        matrix mat;
        mat.Identity();
        mat.Rotate(0,
                   0,
                   angle->y + ((mathutils::frandFrom0To1() * spread) - spread / 2));

        mat.TransformVertex(speedVertex, &speedVector);

        assignParticle(position,
                       speedVector,
                       timeToLive, color, gravity, gridBound, drag, glowPass);
    }
}

void particle::assignParticle(Point3d* position,
                              const Point3d& speedVector,
                              int aTime, vector::pen* aColor, bool gravity, bool gridBound, float drag, bool /*glowPass*/)
{
    PARTICLE* p = &mParticles[mIndex++];
    if (mIndex >= mParticles.size())
        mIndex = 0;

    if (p) {
        Point3d pos = *position;

        if (gridBound) {
            Point3d hitPoint;
            if (theGame->mGrid->hitTest(pos, 0, &hitPoint)) {
                pos = hitPoint;
            }
        }

        for (int i = 0; i < NUM_POS_STREAM_ITEMS; i++) {
            p->posStream[i] = pos;
        }

        p->speedX = speedVector.x;
        p->speedY = speedVector.y;
        p->color = *aColor;
        p->timeToLive = aTime * mathutils::frandFrom0To1();
        p->gravity = gravity;
        p->gridBound = gridBound;
        p->drag = drag;
    }
}

void particle::draw()
{
    // std::unique_lock<std::mutex> lock(m);
    for (auto& p: mParticles) {
        if (p.timeToLive > 0) {
            // This particle is active

            const float speedNormal = mathutils::calculate2dDistance(Point3d(0.0f, 0.0f, 0.0f), Point3d(p.speedX, p.speedY, 0.0f));
            const float a = p.color.a * (speedNormal * 0.8f);

            if (a < 0.05f) {
                // This particle died
                p.timeToLive = 0;
                continue;
            }

            float width = speedNormal * 8.0f;
            if (width > 4.0f)
                width = 4.0f;
            else if (width < 2.0f)
                width = 2.0f;

            if (scene::mPass == scene::RENDERPASS_BLUR) {
                width *= 4.0f;
            }

            glLineWidth(width);

            // This is SO inefficient
            // I really should be doing all the particles in one GL_LINES block but the
            // width of the lines needs to change per-particle, and you can't do a call to
            // glLineWidth() inside a GL_LINES block :-(

            // TODO: use vertex arrays
            glBegin(GL_LINES);

            float aa = (a > 1.0f) ? 1.0f : a;

            for (int i = 0; (i < NUM_POS_STREAM_ITEMS - 1) && (aa > 0.0f); i++) {
                // glColor4f(p->color.r, p->color.g, p->color.b, aa); // RGBA

                const Point3d& from = p.posStream[i];
                const Point3d& to = p.posStream[i + 1];

                glColor4f(p.color.r, p.color.g, p.color.b, aa); // RGBA
                glVertex2d(from.x, from.y);
                aa -= 0.1f;

                glColor4f(p.color.r, p.color.g, p.color.b, aa); // RGBA

                if ((from.x == to.x) && (from.y == to.y)) {
                    glVertex2d(to.x + 0.1f, to.y + 0.1f);
                } else {
                    glVertex2d(to.x, to.y);
                }

                aa -= 0.1f;
            }

            glEnd();
        }
    }
}

void particle::run()
{
    mRunFlag = true;
}

void particle::killAll()
{
    // std::unique_lock<std::mutex> lock(m);

    for (auto& p: mParticles) {
        p.timeToLive = 0;
    }
}
