#include "particle.hpp"
#include "enemies.hpp"
#include "entityblackhole.hpp"
#include "game.hpp"
#include "mathutils.hpp"
#include "profiler.hpp"

#include <atomic>
#include <cstdio>
#include <mutex>

#include "SDL_opengl.h"

static SDL_Thread* mRunThread = NULL;
static std::atomic_bool mRunFlag { false };
static std::atomic_bool quitFlag { false };

std::vector<particle::PARTICLE> particle::mParticles;
int particle::mNumParticles = 0;
int particle::mIndex = 0;

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

        for (int i = 0; i < particle::mNumParticles; i++) {
            particle::PARTICLE* particle = &particle::mParticles[i];

            if (particle->timeToLive > 0) {
                // This particle is active

                if (--particle->timeToLive < 0) {
                    // This particle died
                    particle->timeToLive = 0;
                } else {
                    // Evaluate against attractors
                    if (particle->gravity) {
                        const Point3d speed = game::mAttractors.evaluateParticle(particle);
                        particle->speedX += speed.x;
                        particle->speedY += speed.y;
                    }

                    // Evaluate against black holes
                    for (int j = blackHoleStart; j < blackHoleEnd; j++) {
                        auto blackHole = static_cast<entityBlackHole*>(theGame->mEnemies->mEnemies[j]);
                        if (blackHole->getState() == entity::ENTITY_STATE_RUNNING && blackHole->mActivated) {
                            if (mathutils::calculate2dDistance(particle->posStream[0], blackHole->getPos()) < blackHole->getRadius() * 1.01) {
                                // kill this particle
                                particle->timeToLive *= .7;
                                continue;
                            }
                        }
                    }

                    // Add drag
                    particle->speedX *= particle->drag;
                    particle->speedY *= particle->drag;

                    Point3d speedClamp(particle->speedX, particle->speedY, 0);
                    speedClamp = mathutils::clamp2dVector(speedClamp, 2);
                    particle->speedX = speedClamp.x;
                    particle->speedY = speedClamp.y;

                    // Move the particle
                    particle->posStream[0].x += particle->speedX;
                    particle->posStream[0].y += particle->speedY;
                    particle->posStream[0].z = 0;

                    if (particle->gridBound) {
                        // Keep it within the grid

                        Point3d hitPoint;
                        Point3d speed(particle->speedX, particle->speedY);
                        if (theGame->mGrid->hitTest(particle->posStream[0], 0, &hitPoint, &speed)) {
                            particle->hitGrid = true;
                            particle->posStream[0] = hitPoint;

                            particle->speedX = speed.x;
                            particle->speedY = speed.y;
                        }
                    }

                    // Shift the position stream
                    for (int p = NUM_POS_STREAM_ITEMS - 2; p >= 0; p--) {
                        particle->posStream[p + 1] = particle->posStream[p];
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
    mNumParticles = theGame->mSettings.mParticles; // PERFORMANCE: The larger this number is, the larger the performance hit!
    mIndex = 0;

    mParticles.resize(mNumParticles);

    for (int i = 0; i < mNumParticles; i++) {
        mParticles[i].timeToLive = 0;
    }

    // Thread stuff
    mRunThread = SDL_CreateThread(runThread, "particle", NULL);
    if (!mRunThread) {
#ifdef USE_SDL
        printf("Couldn't create particle run thread: %s\n", SDL_GetError());
#else
        OutputDebugString(L"Couldn't create particle run thread\n");
#endif
    }
}

particle::~particle()
{
    quitFlag = true;

    int status = 0;
    SDL_WaitThread(mRunThread, &status);

    printf("Particle thread exited with status %d\n", status);

    for (int i = 0; i < mNumParticles; i++) {
        mParticles[i].timeToLive = 0;
    }
}

void particle::emitter(Point3d* position, Point3d* angle, float speed, float spread, int num, vector::pen* color, int timeToLive,
                       bool gravity, bool gridBound, float drag, bool glowPass)
{

    // Emit a number of random thrust particles from the nozzle
    for (int p = 0; p < num; p++) {
        Point3d speedVertex, speedVector;

        speedVertex.x = 0;
        speedVertex.y = speed * mathutils::frandFrom0To1();
        speedVertex.z = 0;

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
                              int aTime, vector::pen* aColor, bool gravity, bool gridBound, float drag, bool glowPass)
{
    PARTICLE* particle = &mParticles[mIndex++];
    if (mIndex >= mNumParticles)
        mIndex = 0;

    if (particle) {
        Point3d pos = *position;

        if (gridBound) {
            Point3d hitPoint;
            if (theGame->mGrid->hitTest(pos, 0, &hitPoint)) {
                pos = hitPoint;
            }
        }

        for (int p = 0; p < NUM_POS_STREAM_ITEMS; p++) {
            particle->posStream[p] = pos;
        }

        particle->speedX = speedVector.x;
        particle->speedY = speedVector.y;
        particle->speedZ = speedVector.z;
        particle->color = *aColor;
        particle->timeToLive = aTime * mathutils::frandFrom0To1();
        particle->fadeStep = particle->timeToLive ? 1.0f / particle->timeToLive : 1.0f;
        particle->gravity = gravity;
        particle->gridBound = gridBound;
        particle->drag = drag;
        particle->glowPass = glowPass;
        particle->hitGrid = false;
    }
}

void particle::draw()
{
    // std::unique_lock<std::mutex> lock(m);
    for (int i = 0; i < mNumParticles; i++) {
        PARTICLE* particle = &mParticles[i];

        if (particle->timeToLive > 0) {
            // This particle is active

            const float speedNormal = mathutils::calculate2dDistance(Point3d(0, 0, 0), Point3d(particle->speedX, particle->speedY, particle->speedZ));
            const float a = particle->color.a * (speedNormal * 0.8f);

            if (a < 0.05f) {
                // This particle died
                particle->timeToLive = 0;
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

            for (int p = 0; (p < NUM_POS_STREAM_ITEMS - 1) && (aa > 0.0f); p++) {
                // glColor4f(particle->color.r, particle->color.g, particle->color.b, aa); // RGBA

                const Point3d& from = particle->posStream[p];
                const Point3d& to = particle->posStream[p + 1];

                glColor4f(particle->color.r, particle->color.g, particle->color.b, aa); // RGBA
                glVertex3d(from.x, from.y, 0);
                aa -= 0.1f;

                glColor4f(particle->color.r, particle->color.g, particle->color.b, aa); // RGBA

                if ((from.x == to.x) && (from.y == to.y)) {
                    glVertex3d(to.x + 0.1f, to.y + 0.1f, 0);
                } else {
                    glVertex3d(to.x, to.y, 0);
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

    for (int i = 0; i < mNumParticles; i++) {
        mParticles[i].timeToLive = 0;
    }
}
