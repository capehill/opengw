#include "entitygrunt.h"
#include "game.h"
#include "sincos.h"

entityGrunt::entityGrunt()
    : entity()
{
    mScale = 1.5;
    mRadius = 3;

    mScoreValue = 50;

    mType = ENTITY_TYPE_GRUNT;
    setState(ENTITY_STATE_INACTIVE);

    mAnimationIndex = 0;

    mPen = vector::pen(.5, 1, 1, .7, 12);

    int i = 0;

    mModel.mNumVertex = 4;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, 1);
    mModel.mVertexList[i++] = Point3d(1, 0);
    mModel.mVertexList[i++] = Point3d(0, -1);
    mModel.mVertexList[i++] = Point3d(-1, 0);

    i = 0;

    mModel.mNumEdges = 4;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 0;
}

void entityGrunt::run()
{
    if (this->getEnabled())
    {
        // Seek the player

        float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos());
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        mSpeed += moveVector * .01;
        mSpeed = mathutils::clamp2dVector(mSpeed, .3 * mAggression);

        mSpeed *= .99;

        // Run animation
        mAnimationIndex += .07;
        mScale.x = 2 + (get_sin(mAnimationIndex) * .4);
        mScale.y = 2.5 + (get_sin(-mAnimationIndex) * .4);

    }
    entity::run();
}

void entityGrunt::spawnTransition()
{
    entity::spawnTransition();
    game::mSound.playTrack(SOUNDID_ENEMYSPAWN4);
}