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
/*
                // I'm not sure it's accurate to suck players into black holes. I'll have to double check.
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
//                            player->setState(entity::ENTITY_STATE_DESTROY_TRANSITION);
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
*/

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
                                // It's another black hole. Keep it at the proper distance
                                entityBlackHole* blackHole2 = static_cast<entityBlackHole*>(enemy);
//                                if (blackHole2->mActivated)
                                {
                                    float totalRadius = blackHole->getRadius()+blackHole2->getRadius();

                                    if (distance < totalRadius)
                                    {
/*
                                        if ((totalRadius - distance) > 2)
                                        {
                                            // Combine them
                                            if (blackHole2->mActivated)
                                            {
                                                Point3d midpoint((blackHole->getPos().x + blackHole2->getPos().x)/2, (blackHole->getPos().y + blackHole2->getPos().y)/2, 0);
                                                blackHole->setPos(midpoint);
                                                blackHole->mStrength += blackHole2->mStrength / 2;

                                                blackHole2->setState(entity::ENTITY_STATE_DESTROY_TRANSITION);
                                                game::mSound.playTrack(SOUNDID_GRAVITYWELLABSORBED);

                                                float angle = mathutils::calculate2dAngle(blackHole->getPos(), blackHole2->getPos());
                                                Point3d vector(-1,0,0);
                                                vector = mathutils::rotate2dPoint(vector, angle);
                                                blackHole->setDrift(blackHole->getDrift() + vector * .2);
                                                blackHole->mBalance = 2;

                                            }
                                            else
                                            {
                                                blackHole2->hit(blackHole);
                                            }
                                        }
                                        else
*/
                                        {
                                            // Activate it and nudge each away from each other
                                            if (!blackHole2->mActivated)
                                                blackHole2->hit(blackHole);

                                            float angle1 = mathutils::calculate2dAngle(blackHole2->getPos(), blackHole->getPos());
                                            float angle2 = mathutils::calculate2dAngle(blackHole->getPos(), blackHole2->getPos());

                                            float overlap = totalRadius - distance;

                                            Point3d vector1(overlap/2,0,0);
                                            Point3d vector2(overlap/2,0,0);

                                            vector1 = mathutils::rotate2dPoint(vector1, angle1);
                                            vector2 = mathutils::rotate2dPoint(vector2, angle2);

                                            blackHole->setPos(blackHole->getPos() + vector1);
                                            blackHole2->setPos(blackHole2->getPos() + vector2);

                                        }
                                    }
                                    else
                                    {
                                        float strength = 10;
                                        if (distance < blackHole->getRadius())
                                        {
                                            distance = blackHole->getRadius();
                                        }

                                        float r = 1.0/(distance*distance);

                                        // Add a slight curving vector to the gravity
                                        Point3d gravityVector(r * strength, 0, 0);
                                        Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                                        Point3d speed = enemy->getDrift();
                                        speed.x += g.x;
                                        speed.y += g.y;
                                        enemy->setDrift(speed);
                                    }
                                }

                            }
                            else if ((game::mEnemies.mEnemies[j]->getType() == entity::ENTITY_TYPE_GEOM_SMALL)
                                || (game::mEnemies.mEnemies[j]->getType() == entity::ENTITY_TYPE_GEOM_MEDIUM)
                                || (game::mEnemies.mEnemies[j]->getType() == entity::ENTITY_TYPE_GEOM_LARGE))
                            {
                                // Geoms not effected
                            }
                            else
                            {
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
                                        att->zStrength = 0;
                                        att->radius = 30;
                                        att->pos = blackHole->getPos();
                                        att->enabled = TRUE;
                                        att->attractsParticles = FALSE;
                                    }
                                }
                                else
                                {
                                    float strength = 8;
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
    }

    // Kill any particles that fly into the black hole

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


    // Push missiles away from the black hole
    // This doesn't work correctly. Instead of throwing incoming missiles off course it "repels" them
    // which has the effect of missiles that are directly oncoming being reflected back at their source.
    // What we want is for them to just be thrown off-course. Look into changing the trajectory of the
    // missiles instead of setting the drift on them.

/*
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if ((game::mEnemies.mEnemies[i]->getType() == entity::ENTITY_TYPE_BLACKHOLE) && (game::mEnemies.mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING))
        {
            entityBlackHole* blackHole = static_cast<entityBlackHole*>(game::mEnemies.mEnemies[i]);
            if (blackHole->mActivated)
            {
                for (int i=0; i<game::mPlayers.mPlayer1->mMaxMissiles; i++)
                {
                    entityPlayerMissile* missile = &game::mPlayers.mPlayer1->missiles[i];
                    if (missile->getState() == entity::ENTITY_STATE_RUNNING)
                    {
                        float angle = mathutils::calculate2dAngle(missile->getPos(), blackHole->getPos());
                        float distance = mathutils::calculate2dDistance(missile->getPos(), blackHole->getPos());

                        float strength = -10;
                        if (distance < blackHole->getRadius())
                        {
                            distance = blackHole->getRadius();
                        }

                        float r = 1.0/(distance*distance);

                        Point3d gravityVector(r * strength, 0, 0);
                        Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                        Point3d speed = missile->getDrift();
                        speed.x += g.x;
                        speed.y += g.y;
                        missile->setDrift(speed);
                    }
                }
            }
        }
    }
*/

}






