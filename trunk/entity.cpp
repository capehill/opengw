#include "entity.h"

#include "game.h"
#include "entityPlayer1.h"
#include "entityPlayer2.h"
#include "entityplayermissile.h"
#include "entitygrunt.h"
#include "entitywanderer.h"
#include "entityweaver.h"
#include "entityspinner.h"
#include "entitytinyspinner.h"
#include "entitymayfly.h"
#include "entitysnake.h"
#include "entityblackhole.h"
#include "entityrepulsor.h"
#include "entityProton.h"
#include "entityLine.h"

entity::entity()
    : mType(ENTITY_TYPE_UNDEF)
{
    setState(ENTITY_STATE_INACTIVE);
    mSpawnTime = 60;
    mDestroyTime = 3;
    mIndicateTime = 75;
    mStateTimer = 0;
    mAggression = 1;
    mEdgeBounce = FALSE;
    mGridBound = TRUE;
    mGenId = 0;
}

// Static class factory
entity* entity::createEntity(EntityType _entity)
{
    switch (_entity)
    {
        case ENTITY_TYPE_PLAYER1:
            return new entityPlayer1();
        case ENTITY_TYPE_PLAYER2:
            return new entityPlayer2();
        case ENTITY_TYPE_PLAYER_MISSILE:
            return new entityPlayerMissile();
        case ENTITY_TYPE_GRUNT:
            return new entityGrunt();
        case ENTITY_TYPE_WANDERER:
            return new entityWanderer();
        case ENTITY_TYPE_WEAVER:
            return new entityWeaver();
        case ENTITY_TYPE_SPINNER:
            return new entitySpinner();
        case ENTITY_TYPE_TINYSPINNER:
            return new entityTinySpinner();
        case ENTITY_TYPE_MAYFLY:
            return new entityMayfly();
        case ENTITY_TYPE_SNAKE:
            return new entitySnake();
        case ENTITY_TYPE_BLACKHOLE:
            return new entityBlackHole();
        case ENTITY_TYPE_REPULSOR:
            return new entityRepulsor();
        case ENTITY_TYPE_PROTON:
            return new entityProton();
        case ENTITY_TYPE_LINE:
            return new entityLine();
        default:
            return new entity();
    }

    return 0;
}

void entity::draw()
{
    mModel.Identity();
    mModel.Scale(mScale);
    mModel.Rotate(mAngle);
    mModel.Translate(mPos);

    if (this->getState() == entity::ENTITY_STATE_INDICATING)
    {
        if (((int)(mStateTimer/5)) & 1)
        {
            vector::pen pen = mPen;
            mModel.draw(pen);
        }
    }
    else if (this->getEnabled() && (this->getState() != entity::ENTITY_STATE_SPAWN_TRANSITION))
    {
        vector::pen pen = mPen;

        if (getState() == ENTITY_STATE_SPAWNING)
        {
            Point3d scale = mScale;
            Point3d trans = mPos;

            float inc = 1.0f / mSpawnTime;
            float progress = mStateTimer * inc;

            // *********************************************

		    glLineWidth(pen.lineRadius*.3);
			glBegin(GL_LINES);

            progress = 1-progress;
			
            float a = progress;
            if (a<0) a = 0;
            if (a>1) a = 1;
			
            pen.a = a;

            mModel.Identity();
            mModel.Scale(scale * progress * 1);
            mModel.Rotate(mAngle);
            mModel.Translate(trans);
            mModel.emit(pen);
			
            // *********************************************

            progress = progress + .25;

            a = 1-progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            pen.a = a;

            mModel.Identity();
            mModel.Scale(scale * progress * 4);
            mModel.Rotate(mAngle);
            mModel.Translate(trans);
            mModel.emit(pen);

            // *********************************************

            progress = progress + .25;

            a = 1-progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            pen.a = a;

            mModel.Identity();
            mModel.Scale(scale * progress * 7);
            mModel.Rotate(mAngle);
            mModel.Translate(trans);
            mModel.emit(pen);

            // *********************************************

            // Restore stuff
            mModel.Identity();
            mModel.Rotate(mAngle);
            mModel.Scale(scale);
            mModel.Translate(trans);

			glEnd();
        }

        mModel.draw(pen);

    }
}

void entity::runTransition()
{
    setState(ENTITY_STATE_RUNNING);
}

void entity::run()
{
    mAggression += .0002;

    mPos += mSpeed;
    mPos += mDrift;
    mAngle += mRotationRate;
    mAngle = fmodf(mAngle, 2.0f*PI);

    // Keep it on the grid
    if (mGridBound)
    {
	    Point3d hitPoint;
	    Point3d speed = mSpeed;
	    if (game::mGrid.hitTest(mPos, mRadius, &hitPoint, &speed))
	    {
		    mPos = hitPoint;
            if (mEdgeBounce)
            {
                mSpeed = speed;
            }
	    }
    }

    // Update the model's matrix
    mModel.Identity();
    mModel.Scale(mScale);
    mModel.Rotate(mAngle);
    mModel.Translate(mPos);

    mDrift *= .95;
}

void entity::spawnTransition()
{
    setState(ENTITY_STATE_SPAWNING);
    mStateTimer = mSpawnTime;
    mSpeed = Point3d(0,0,0);
    mDrift = Point3d(0,0,0);
    mAngle = 0;
    mRotationRate = 0;
    mAggression = 1;

    // Update the model's matrix
    mModel.Identity();
    mModel.Scale(mScale);
    mModel.Rotate(mAngle);
    mModel.Translate(mPos);

    spawn();
}

void entity::spawn()
{
    if (--mStateTimer <= 0)
    {
        setState(ENTITY_STATE_RUNNING);
    }
}

void entity::destroyTransition()
{
    setState(ENTITY_STATE_DESTROYED);
    mStateTimer = mDestroyTime;

    ++mGenId;

    // Explode the object into line entities
    game::mEnemies.explodeEntity(*this);
}

void entity::destroy()
{
    if (--mStateTimer <= 0)
    {
        setState(ENTITY_STATE_INACTIVE);
        return;
    }

    // Throw out some particles
    Point3d pos(this->mPos);
    Point3d angle(0,0,0);
    float speed = 3.5;
    float spread = 2*PI;
    int num = 50;
    int timeToLive = 200;
    vector::pen pen = mPen;
    pen.r *= 1.2;
    pen.g *= 1.2;
    pen.b *= 1.2;
    pen.a = .9;
    pen.lineRadius=5;
    game::mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .96, TRUE);
}

void entity::indicateTransition()
{
    mStateTimer = mIndicateTime;
    setState(ENTITY_STATE_INDICATING);
}

void entity::indicating()
{
    if (--mStateTimer <= 0)
    {
        setState(ENTITY_STATE_INACTIVE);
    }
}

void entity::hit(entity* aEntity)
{
    if (aEntity)
    {
        entityPlayerMissile* missile = dynamic_cast<entityPlayerMissile*>(aEntity);
        if (missile)
        {
            if (mScoreValue)
            {
                // Add points and display them at the destruction point
                if (missile->mPlayerSource == 0)
                    game::mPlayers.mPlayer1->addKillAtLocation(mScoreValue, getPos());
                else if (missile->mPlayerSource == 1)
                    game::mPlayers.mPlayer2->addKillAtLocation(mScoreValue, getPos());
                else if (missile->mPlayerSource == 2)
                    game::mPlayers.mPlayer3->addKillAtLocation(mScoreValue, getPos());
                else if (missile->mPlayerSource == 3)
                    game::mPlayers.mPlayer4->addKillAtLocation(mScoreValue, getPos());
            }

            game::mSound.playTrack(SOUNDID_ENEMYHIT);
        }
        else if (aEntity && aEntity->getType() == entity::ENTITY_TYPE_BLACKHOLE)
        {
            game::mSound.playTrack(SOUNDID_ENEMYHIT);
        }
        else if (aEntity && (aEntity->getType() == entity::ENTITY_TYPE_PLAYER1) || (aEntity->getType() == entity::ENTITY_TYPE_PLAYER2))
        {
            game::mSound.playTrack(SOUNDID_ENEMYHIT);
        }
    }
    else
    {
        // It must be either a bomb or the player getting destroyed
    }

    setState(ENTITY_STATE_DESTROY_TRANSITION);
}

entity* entity::hitTest(const Point3d& pos, float radius)
{
    Point3d ourPos(0,0,0);
    this->getModel()->mMatrix.TransformVertex(ourPos, &ourPos);

    float distance = mathutils::calculate2dDistance(pos, ourPos);
    float resultRadius = radius + getRadius();
    if (distance < resultRadius)
    {
        return this;
    }
    return NULL;
}





