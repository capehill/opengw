#include "entityplayermissile.hpp"
#include "enemies.hpp"
#include "entityblackhole.hpp"
#include "entityrepulsor.hpp"
#include "game.hpp"
#include "players.hpp"

entityPlayerMissile::entityPlayerMissile()
    : entity()
{
    mScale = .16;
    mRadius = 1;

    mPlayerSource = 1;

    mScoreValue = 0;

    mSpawnTime = 0;
    mDestroyTime = 0;

    mType = ENTITY_TYPE_PLAYER_MISSILE;

    int i = 0;

    mModel.mVertexList.resize(3);
    mModel.mVertexList[i++] = Point3d(0, 7.5);
    mModel.mVertexList[i++] = Point3d(2, -5.2);
    mModel.mVertexList[i++] = Point3d(-2, -5.2);

    i = 0;

    mModel.mEdgeList.resize(3);
    mModel.mEdgeList[i].from = 0;
    mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1;
    mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2;
    mModel.mEdgeList[i++].to = 0;
}

void entityPlayerMissile::run()
{
    mLastPos = mPos;

    // Check for repulsors that may effect us
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if ((theGame->mEnemies->mEnemies[i]->getType() == entity::ENTITY_TYPE_REPULSOR) && (theGame->mEnemies->mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING)) {
            entityRepulsor* repulsor = static_cast<entityRepulsor*>(theGame->mEnemies->mEnemies[i]);
            if (repulsor) {
                repulsor->repelEntity(this);
            }
        }
    }

    // Check for black holes that may effect us
    bool warped = false;
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if ((theGame->mEnemies->mEnemies[i]->getType() == entity::ENTITY_TYPE_BLACKHOLE) && (theGame->mEnemies->mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING)) {
            entityBlackHole* blackHole = static_cast<entityBlackHole*>(theGame->mEnemies->mEnemies[i]);
            if (blackHole->mActivated) {
                float distance = mathutils::calculate2dDistance(mPos, blackHole->getPos());
                if (distance < 20) {
                    Point3d v2 = blackHole->getPos();
                    Point3d v1 = mPos;

                    float angle = mathutils::wrapRadians(mathutils::calculate2dAngle(v1, v2));
                    float heading = mathutils::wrapRadians(getAngle() + mathutils::DegreesToRads(90));

                    float targetingAngle = mathutils::diffAngles(angle, heading);
                    float targetingOffset = fabs(targetingAngle);

                    if (targetingOffset < 0.8) {
                        mRotationRate += targetingAngle * .1;
                        warped = true;
                    }
                }
            }
        }
    }

    mRotationRate *= .5;

    if (warped) {
        Point3d vector(mVelocity, 0, 0);
        mSpeed = mathutils::rotate2dPoint(vector, mAngle + mathutils::DegreesToRads(90));
    }

    mPos += mSpeed;
    mPos += mDrift;
    mAngle -= mRotationRate;
    mAngle = fmodf(mAngle, 2.0f * PI);

    // Update the model's matrix
    mModel.Identity();
    mModel.Scale(mScale);
    mModel.Rotate(mAngle);
    mModel.Translate(mPos);

    mDrift *= .95;

    if (this->getEnabled()) {
        // Hit test 10 interpolated positions against enemies
        bool hit = false;
        for (int i = 0; i < 10; i++) {
            float amount1 = i / 10.0f;
            float amount2 = 1 - amount1;

            Point3d pos((mPos.x * amount1) + (mLastPos.x * amount2), (mPos.y * amount1) + (mLastPos.y * amount2), 0);

            entity* enemy = theGame->mEnemies->hitTestEnemiesAtPosition(pos, getRadius(), true);
            if (enemy) {
                // We hit an enemy - destroy it and ourselves
                setState(ENTITY_STATE_DESTROY_TRANSITION);
                enemy->hit(this);
            }
        }

        if (!hit) {
            if (theGame->mGrid->hitTest(mPos, 0)) {
                // Hit the edge of the grid - destroy it
                setState(ENTITY_STATE_DESTROY_TRANSITION);
                mSpeed = Point3d(0, 0, 0);

                theGame->mSound->playTrack(SOUNDID_MISSILEHITWALL);
            }

            // Here be an attractor
            if (mType == 0) {
                attractor::Attractor* att = game::mAttractors.getAttractor();
                if (att) {
                    att->strength = 10;
                    att->radius = 5;
                    att->pos = mPos;
                    att->enabled = true;
                    att->attractsParticles = false;
                }
            } else if (mType == 1) {
                attractor::Attractor* att = game::mAttractors.getAttractor();
                if (att) {
                    att->strength = 20;
                    att->radius = 5;
                    att->pos = mPos;
                    att->enabled = true;
                    att->attractsParticles = false;
                }
            } else if (mType == 2) {
                attractor::Attractor* att = game::mAttractors.getAttractor();
                if (att) {
                    att->strength = 30;
                    att->radius = 5;
                    att->pos = mPos;
                    att->enabled = true;
                    att->attractsParticles = false;
                }
            }
        }
    }
}

void entityPlayerMissile::spawnTransition()
{
    spawn();
    setState(ENTITY_STATE_RUNNING);
    mRotationRate = 0;
    run();
}

void entityPlayerMissile::destroyTransition()
{
    mStateTimer = mDestroyTime;
    entity::destroy();

    setState(ENTITY_STATE_INACTIVE); // kill it off immediately

    // Throw out some particles
    Point3d pos(this->mPos);
    Point3d angle(0, 0, 0);
    float speed = 2;
    float spread = 2 * PI;
    int num = 20;
    int timeToLive = 200;
    vector::pen pen = mPen;
    pen.lineRadius = 5;
    pen.a = .7;
    theGame->mParticles->emitter(&pos, &angle, speed, spread, num, &pen, timeToLive);
}

void entityPlayerMissile::draw()
{
    if (getState() == entity::ENTITY_STATE_RUNNING) {
        // Set the proper color for the player
        switch (mPlayerSource) {
        case 0:
            mPen = theGame->getPlayer1()->getMissilesPen();
            break;
        case 1:
            mPen = theGame->getPlayer2()->getMissilesPen();
            break;
        case 2:
            mPen = theGame->getPlayer3()->getMissilesPen();
            break;
        case 3:
            mPen = theGame->getPlayer4()->getMissilesPen();
            break;
        }

        mPen.lineRadius = 12;

        entity::draw();
    }
}
