#include "defines.hpp"
#include "entityproton.hpp"
#include "game.hpp"
#include "players.hpp"

entityProton::entityProton(void)
{
    mScale = 1;
    mRadius = 1.2;

    mEdgeBounce = true;

    mSpawnTime = 0;

    mScoreValue = 50; // ??

    mDestroyTime = 0;

    mPen = vector::pen(.5, .6, 1, .7, 12);

    mType = ENTITY_TYPE_PROTON;
    setState(ENTITY_STATE_INACTIVE);

    mModel.mVertexList.resize(16);

    float delta_theta = (2 * PI) / mModel.mVertexList.size();
    float r = mScale.x * mRadius;

    std::size_t i = 0;
    for (float angle = 0; i < mModel.mVertexList.size(); angle += delta_theta, i++) {
        mModel.mVertexList[i] = Point3d(r * cos(angle), r * sin(angle));
    }

    mModel.mEdgeList.resize(mModel.mVertexList.size());

    for (i = 0; i < mModel.mEdgeList.size() - 1; ++i) {
        mModel.mEdgeList[i].from = i;
        mModel.mEdgeList[i].to = i + 1;
    }
    mModel.mEdgeList[i].from = i;
    mModel.mEdgeList[i].to = 0;
}

void entityProton::run()
{
    if (this->getEnabled()) {
        // Seek the player

        float angle = mathutils::calculate2dAngle(mPos, theGame->mPlayers->getPlayerClosestToPosition(mPos)->getPos());
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        mSpeed += moveVector * .1;
        mSpeed = mathutils::clamp2dVector(mSpeed, .6 * mAggression);

        //        mSpeed *= .98;
    }
    entity::run();
}

void entityProton::spawnTransition()
{
    entity::spawnTransition();
    mDrift.x = (mathutils::frandFrom0To1() * 4) - 2;
    mDrift.y = (mathutils::frandFrom0To1() * 4) - 2;
}
