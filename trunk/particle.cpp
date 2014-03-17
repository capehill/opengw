#include "particle.h"
#include "game.h"

#define VIRTUAL_SCREEN_WIDTH 800 // FIX ME
#define VIRTUAL_SCREEN_HEIGHT 600 // FIX ME

#define _DISTANCE 320
#define _MIN_DISTANCE .001
#define _MAX_DISTANCE 800


static BOOL renderParticles = TRUE;
static float brightness = 1.0f;
static float particleLineRadius = 3;


particle::particle()
{
    mNumParticles = 1000; // PERFORMANCE: The larger this number is, the larger the performance hit!

    mParticles = new PARTICLE[mNumParticles];
    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            mParticles[i].timeToLive = 0;
        }
    }
}

particle::~particle()
{
    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            mParticles[i].timeToLive = 0;
        }

        delete mParticles;
    }
    mParticles = NULL;
}


void particle::emitter(Point3d* position, Point3d* angle, float speed, float spread, int num, vector::pen* color, int timeToLive,
                       BOOL gravity, BOOL gridBound, float drag, BOOL glowPass)
{

    // Emit a number of random thrust particles from the nozzle
    for (int p=0; p<num; p++)
    {
        Point3d speedVertex, speedVector;

        //    timeToLive *= .98;

        speedVertex.x = 0;
        speedVertex.y = speed + ((mathutils::frandFrom0To1()*.5)-.25);
        speedVertex.z = 0;

        matrix mat;
        mat.Identity();
        mat.Rotate(0,
                   0,
                   angle->y + ((mathutils::frandFrom0To1()*spread)-spread/2));

        mat.TransformVertex(speedVertex, &speedVector);

        assignParticle(position,
                       speedVector.x, speedVector.y, speedVector.z,
                       timeToLive, color, gravity, gridBound, drag, glowPass);

    }
}


void particle::assignParticle(Point3d* position,
                              float aSpeedX, float aSpeedY, float aSpeedZ,
                              int aTime, vector::pen* aColor, BOOL gravity, BOOL gridBound, float drag, BOOL glowPass)
{
    int i;

    // Find an unused particle
    PARTICLE* particle = NULL;

    if (mParticles)
    {
        for (i=0; i<mNumParticles; i++)
        {
            if (mParticles[i].timeToLive <= 0)
            {
                // Found one
                particle = &mParticles[i];
                break;
            }
        }
    }

    if (!particle)
    {
        // Out of particles - pick off the particle closest to being done
        int minTime = 320000;
        int minIndex = 0;

        for (i=0; i<mNumParticles; i++)
        {
            if (mParticles[i].timeToLive <= minTime)
            {
                minIndex = i;
                minTime = mParticles[i].timeToLive;
            }
        }

        particle = &mParticles[minIndex];
    }

    if (particle)
    {
        Point3d pos = *position;

        if (gridBound)
        {
			Point3d hitPoint;
			if (game::mGrid.hitTest(pos, 0, &hitPoint))
			{
				pos = hitPoint;
			}
        }

        for (int p=0; p<NUM_POS_STREAM_ITEMS; p++)
        {
            particle->posStream[p] = pos;
        }

        particle->speedX = aSpeedX;
        particle->speedY = aSpeedY;
        particle->speedZ = aSpeedZ;
        particle->color = *aColor;
        particle->timeToLive = aTime * mathutils::frandFrom0To1();
        particle->fadeStep = 1.0 / particle->timeToLive;
        particle->gravity = gravity;
        particle->gridBound = gridBound;
        particle->drag = drag;
        particle->glowPass = glowPass;
        particle->hitGrid = false;
    }

}


void particle::draw()
{
    if (mParticles)
    {
        glLineWidth(mParticles[0].color.lineRadius);

        glBegin(GL_LINES);

        for (int i=0; i<mNumParticles; i++)
        {
            if (mParticles[i].timeToLive > 0)
            {
                // This particle is active
                PARTICLE* particle = &mParticles[i];

                float a = particle->color.a;
                float speedNormal = mathutils::calculate2dDistance(Point3d(0,0,0), Point3d(particle->speedX, particle->speedY, particle->speedZ));
                a = a * (speedNormal * .8);

                glColor4f(particle->color.r, particle->color.g, particle->color.b, a); // RGBA

                for (int p=0; p<NUM_POS_STREAM_ITEMS-1; p++)
                {
                    Point3d from = particle->posStream[p];
                    Point3d to = particle->posStream[p+1];

                    vector::pen pen = particle->color;
                    pen.lineRadius *= pen.a;


                    glVertex3d(from.x, from.y, 0);
                    glVertex3d(to.x, to.y, 0);
                }
            }
        }

	    glEnd();

    }

}


void particle::run()
{
    int i;
    if (mParticles)
    {
        for (i=0; i<mNumParticles; i++)
        {
            PARTICLE* particle = &mParticles[i];
            if (particle->timeToLive > 0)
            {
                // This particle is active
                --particle->timeToLive;
                if (particle->timeToLive <= 0)
                {
                    // This particle died
                    particle->timeToLive = 0;
                }
                else
                {
                    // Evaluate against attractors
                    Point3d ppoint = particle->posStream[0];
                    if (particle->gravity)
                    {
                        Point3d speed = game::mAttractors.evaluateParticle(particle);
                        particle->speedX += speed.x;
                        particle->speedY += speed.y;
                    }

                    // Add drag
                    particle->speedX *= particle->drag;
                    particle->speedY *= particle->drag;
                
                    Point3d speedClamp(particle->speedX, particle->speedY, 0);
                    speedClamp = mathutils::clamp2dVector(speedClamp, 2);
                    particle->speedX = speedClamp.x;
                    particle->speedY = speedClamp.y;

/*
                    if (particle->hitGrid)
                    {
                        float angle = mathutils::frandFrom0To1() * (2*PI);
                        Point3d vector(particle->speedX, particle->speedY, 0);
                        vector = mathutils::rotate2dPoint(vector, angle);

                        particle->speedX = vector.x*.9;
                        particle->speedY = vector.y*.9;
                    }
*/

                    // Move the particle
                    particle->posStream[0].x += particle->speedX;
                    particle->posStream[0].y += particle->speedY;
                    particle->posStream[0].z = 0;

                    if (particle->gridBound/* && !particle->hitGrid*/)
                    {
                        // Keep it within the grid

						Point3d hitPoint;
                        Point3d speed(particle->speedX, particle->speedY);
						if (game::mGrid.hitTest(particle->posStream[0], 0, &hitPoint, &speed))
						{
                            particle->hitGrid = true;
							particle->posStream[0] = hitPoint;

                            particle->speedX = speed.x;
                            particle->speedY = speed.y;
						}

                    }

                    // Shift the position stream
                    for (int p=NUM_POS_STREAM_ITEMS-2; p >= 0; p--)
                    {
                        particle->posStream[p+1] = particle->posStream[p];
                    }

                }
            }
        }
    }
}

void particle::killAll()
{
    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            mParticles[i].timeToLive = 0;
        }
    }
}




