#include "blackholes.h"
#include "game.h"
#include "entityBlackHole.h"
#include "particle.h"

blackholes::blackholes()
{
}

void blackholes::run()
{
    // Suck enemies and particles into the black holes
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if ((game::mEnemies.mEnemies[i]->getType() == entity::ENTITY_TYPE_BLACKHOLE) && (game::mEnemies.mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING))
        {
            entityBlackHole* blackHole = static_cast<entityBlackHole*>(game::mEnemies.mEnemies[i]);
            if (blackHole->mActivated)
            {
                //
                // Players
                for (int p=0; p<2; p++)
                {
                    entity* player = (p==0) ? game::mPlayers.mPlayer1 : game::mPlayers.mPlayer2;
                    if (player->getEnabled())
                    {
                        float angle = mathutils::calculate2dAngle(player->getPos(), blackHole->getPos());
                        float distance = mathutils::calculate2dDistance(player->getPos(), blackHole->getPos());

                        if (distance < blackHole->getRadius())
                        {
                            // Destroy the player
                        }
                        else
                        {
                            float strength = 2.4;
                            if (distance < blackHole->getRadius())
                            {
                                distance = blackHole->getRadius();
                            }

                            float r = 1.0/(distance*distance);

                            Point3d gravityVector(r * strength, 0, 0);
                            Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                            Point3d speed = player->getDrift();
                            speed.x += g.x;
                            speed.y += g.y;
                            player->setDrift(speed);
                        }
                    }
                }

                for (int j=0; j<NUM_ENEMIES; j++)
                {
                    if ((game::mEnemies.mEnemies[j]->getState() == entity::ENTITY_STATE_RUNNING))
                    {
                        if (j != i)
                        {
                            entity* enemy = game::mEnemies.mEnemies[j];

                            float angle = mathutils::calculate2dAngle(enemy->getPos(), blackHole->getPos());
                            float distance = mathutils::calculate2dDistance(enemy->getPos(), blackHole->getPos());

                            if (game::mEnemies.mEnemies[j]->getType() == entity::ENTITY_TYPE_BLACKHOLE)
                            {
                                // It's another black hole. Keep it at the proper distance or combine it

                                entityBlackHole* blackHole2 = static_cast<entityBlackHole*>(enemy);
//                                if (blackHole2->mActivated)
                                {
                                    float totalRadius = blackHole->getRadius()+blackHole2->getRadius();
                                    if (distance < 6)
                                    {
                                        // Combine both black holes

                                        Point3d avgPos = (blackHole->getPos() + blackHole2->getPos()) * .5;
                                        float strength = max(blackHole->mStrength, blackHole2->mStrength);
                                        int feedCount = blackHole->mFeedCount + blackHole2->mFeedCount;

                                        // Upgrade the first one
                                        blackHole->setPos(avgPos);
                                        blackHole->mStrength = strength;
                                        blackHole->mFeedCount = feedCount;
                                        blackHole->feed(blackHole2->mPoints);
                                        blackHole->feed(0);

                                        // Kill the second one
                                        blackHole2->setState(entity::ENTITY_STATE_DESTROY_TRANSITION);

                                        game::mSound.playTrack(SOUNDID_GRAVITYWELLHIT);
                                    }
                                    else if (distance < totalRadius)
                                    {
                                        // Push them away from each other if they get too close

                                        float strength = 6;
                                        if (distance < blackHole->getRadius())
                                        {
                                            distance = blackHole->getRadius();
                                        }

                                        float r = 1.0/(distance*distance);

                                        Point3d gravityVector(r * -strength, 0, 0);
                                        Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                                        Point3d speed = enemy->getDrift();
                                        speed.x += g.x;
                                        speed.y += g.y;
                                        enemy->setDrift(speed);

                                    }
                                }

                            }

                            if (distance < blackHole->getRadius())
                            {
                                // Add its drift to ours
                                Point3d drift = enemy->getDrift();
                                drift *= .25;
                                blackHole->setDrift(blackHole->getDrift() + drift);

                                // Destroy the enemy
                                enemy->hit(blackHole);

                                // Feed the black hole
                                blackHole->feed(enemy->getScoreValue());

                                // Distrupt the grid at the destruction point
                                attractor::Attractor* att = game::mAttractors.getAttractor();
                                if (att)
                                {
                                    att->strength = 1.5;
                                    att->radius = 30;
                                    att->pos = blackHole->getPos();
                                    att->enabled = TRUE;
                                    att->attractsParticles = FALSE;
                                }
                            }
                            else
                            {
                                float strength = 16;
                                if (distance < blackHole->getRadius())
                                {
                                    distance = blackHole->getRadius();
                                }

                                float r = 1.0/(distance*distance);

                                // Add a slight curving vector to the gravity
                                Point3d gravityVector(r * strength, 0, 0);
                                Point3d g = mathutils::rotate2dPoint(gravityVector, angle+.4);

                                Point3d speed = enemy->getDrift();
                                speed.x += g.x;
                                speed.y += g.y;
                                enemy->setDrift(speed);
                            }
                        }
                    }
                }
            }
        }
    }

    // Kill any particles that fly into the black hole
/*
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if ((game::mEnemies.mEnemies[i]->getType() == entity::ENTITY_TYPE_BLACKHOLE) && (game::mEnemies.mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING))
        {
            entityBlackHole* blackHole = static_cast<entityBlackHole*>(game::mEnemies.mEnemies[i]);
            if (blackHole->mActivated)
            {
                for (int p=0; p<game::mParticles.mNumParticles; p++)
                {
                    if (game::mParticles.mParticles[p].timeToLive > 0)
                    {
                        // This particle is active
                        particle::PARTICLE* particle = &game::mParticles.mParticles[p];
                        if (particle->gravity)
                        {
                            float distance = mathutils::calculate2dDistance(particle->posStream[0], blackHole->getPos());
                            if (distance < blackHole->getRadius())
                            {
                                particle->timeToLive = 0;
                            }
                        }
                    }
                }
            }
        }
    }
*/

}






