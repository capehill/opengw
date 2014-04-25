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
    mNumParticles = 2000; // PERFORMANCE: The larger this number is, the larger the performance hit!
    mIndex = 0;

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

        speedVertex.x = 0;
        speedVertex.y = speed * mathutils::frandFrom0To1();
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

    PARTICLE* particle = &mParticles[mIndex++];
    if (mIndex >= mNumParticles) mIndex = 0;

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
        for (int i=0; i<mNumParticles; i++)
        {
            if (mParticles[i].timeToLive > 0)
            {
                // This particle is active
                PARTICLE* particle = &mParticles[i];

                float a = particle->color.a;
                float speedNormal = mathutils::calculate2dDistance(Point3d(0,0,0), Point3d(particle->speedX, particle->speedY, particle->speedZ));
                a = a * (speedNormal * .8);

                if (a < .05)
                {
                    // This particle died
                    particle->timeToLive = 0;
                    continue;
                }

                float width = speedNormal * 8;
                if (width > 5) width = 5;

                glColor4f(particle->color.r, particle->color.g, particle->color.b, a); // RGBA
                glLineWidth(width);

                // This is SO inefficient
                // I really should be doing all the particles in one GL_LINES block but the
                // width of the lines needs to change per-particle, and you can't do a call to
                // glLineWidth() inside a GL_LINES block :-(

                glBegin(GL_LINES);

                for (int p=0; p<NUM_POS_STREAM_ITEMS-1; p++)
                {
                    Point3d from = particle->posStream[p];
                    Point3d to = particle->posStream[p+1];

                    glVertex3d(from.x, from.y, 0);
                    glVertex3d(to.x, to.y, 0);
                }

        	    glEnd();

            }
        }


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




