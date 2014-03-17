#include "entitygeom.h"
#include "game.h"


entityGeom::entityGeom()
    : entity()
{
    setState(ENTITY_STATE_INACTIVE);

    mEdgeBounce = true;

    mLifetime = 0;

    mPen = vector::pen(1, 1, 0, .7, 12);
}

void entityGeom::run()
{
    --mLifetime;
    if (mLifetime <= 0)
    {
        mState = ENTITY_STATE_INACTIVE;
    }

    if (this->getEnabled())
    {
        // Suck the geoms into the player when in proximity
        player* player = game::mPlayers.getPlayerClosestToPosition(mPos);
        if (player)
        {
            float distance = mathutils::calculate2dDistance(player->getPos(), mPos);
            if (distance < 17)
            {
                mLifetime = 250;
                if (distance < 4)
                {
                    mState = ENTITY_STATE_INACTIVE;
                    player->addGeom(mScoreValue);
                    player->addKillAtLocation(mScoreValue, mPos);
                }
                else
                {
                    float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos());
                    Point3d moveVector(2, 0, 0);
                    moveVector = mathutils::rotate2dPoint(moveVector, angle);
                    mSpeed += moveVector * .2;
                    mSpeed *= .8;
                }
            }
        }
    }

    entity::run();
}

void entityGeom::draw()
{
    // Flicker as the Geom burns out
    float a = .7;
    if (mLifetime < 100)
    {
        a = (mLifetime / 200.0f) * .7;
    }
    if (mLifetime < 200)
    {
        if (mLifetime/2 & 1) a /= 2;
    }
    mPen = vector::pen(1, 1, 0, a, 12);

    entity::draw();
}

void entityGeom::spawnTransition()
{
    mLifetime = 350;
    mState = ENTITY_STATE_RUNNING;
}
