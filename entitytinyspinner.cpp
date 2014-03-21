#include "entityTinySpinner.h"
#include "game.h"


entityTinySpinner::entityTinySpinner()
    : entitySpinner()
{
    mScale = 1;
    mRadius = 2.2;

    mScoreValue = 50;

    mVirtualPos = Point3d(0,0,0);
    mAnimationIndex = 0;

    mType = ENTITY_TYPE_TINYSPINNER;
}


void entityTinySpinner::draw()
{
    if (this->getState() == entity::ENTITY_STATE_INDICATING)
    {
        if (((int)(mStateTimer/10)) & 1)
        {
            vector::pen pen = mPen;
            mModel.draw(pen);
        }
    }
    else if (this->getEnabled())
    {
        mModel.Identity();
        mModel.Scale(mScale);
        mModel.Rotate(mAngle);
        mModel.Translate(mVirtualPos);

        mModel.draw(mPen);
    }
}


void entityTinySpinner::run()
{
    if (this->getEnabled())
    {
        // Seek the player

        float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos());
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        mSpeed += moveVector * .02;
        mSpeed = mathutils::clamp2dVector(mSpeed, .2 * mAggression);
        mSpeed *= .9;

        // Run circling animation

        mAnimationIndex += .1;
        Point3d offset(4,0,0);
        offset = mathutils::rotate2dPoint(offset, mAnimationIndex);
        mVirtualPos = mPos + offset;
        mAngle = mAnimationIndex*2;
    }
    entity::run();
}

void entityTinySpinner::spawnTransition()
{
    entity::spawnTransition();
}

void entityTinySpinner::spawn()
{
    entity::spawn();

    if (mStateTimer > 0)
    {
        // Make them invincible (and brighter) for a short period of time
        // so we don't just pick them off as soon as they spawn
        float c = mStateTimer / mSpawnTime;
        c = (1 * c) + (.5 * (1-c));
        mPen = vector::pen(1, c, 1, .5, 12);
        mStateTimer -= 1;
        run();
    }
    if (mStateTimer <= 0)
    {
        mPen = vector::pen(1, .5, 1, .5, 12);
    }
}


void entityTinySpinner::destroyTransition()
{
    entity::destroyTransition();
}

void entityTinySpinner::destroy()
{
    entity::destroy();
}






