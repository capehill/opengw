#include "entityMayfly.h"
#include "game.h"

entityMayfly::entityMayfly()
    : entity()
{
    mScale = 0.8;
    mRadius = 3;

    mScoreValue = 50;

    mType = ENTITY_TYPE_MAYFLY;
    setState(ENTITY_STATE_INACTIVE);

    mFlipTimer = mathutils::frandFrom0To1() * 15;
    mFlipDirection = 1;

    mPen = vector::pen(1, .5, 1, .7, 12);

    int i = 0;

    mModel.mNumVertex = 6;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];

    mModel.mVertexList[i++] = Point3d(-.25, 1.25);
    mModel.mVertexList[i++] = Point3d(.25, 1.25);

    mModel.mVertexList[i++] = Point3d(1.2, -.5);
    mModel.mVertexList[i++] = Point3d(1, -.9);

    mModel.mVertexList[i++] = Point3d(-1, -.9);
    mModel.mVertexList[i++] = Point3d(-1.2, -.5);

    i = 0;

    mModel.mNumEdges = 3;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 5;
}

void entityMayfly::run()
{
    if (this->getEnabled())
    {
        // Seek the player

        // Run animation
        if (--mFlipTimer <= 0)
        {
            mFlipTimer = 15;
            mFlipDirection = -mFlipDirection;

            // Update the target
            mTarget = game::mPlayers.getPlayerClosestToPosition(mPos)->getPos();
            mTarget.x += (mathutils::frandFrom0To1() * 30) - 15;
            mTarget.y += (mathutils::frandFrom0To1() * 30) - 15;
        }

        float desiredAngle = 1.2 * mFlipDirection;
        float diff = desiredAngle-mAngle;
        mRotationRate += diff * .03;
        mRotationRate *= .9;

        float angle = mathutils::calculate2dAngle(mPos, mTarget);
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        mSpeed += moveVector * .5;
        mSpeed = mathutils::clamp2dVector(mSpeed, .45 * mAggression);

        mSpeed *= .9;
    }
    entity::run();
}

