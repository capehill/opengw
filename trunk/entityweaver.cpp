#include "entityweaver.h"
#include "game.h"
#include "entityPlayer1.h"


entityWeaver::entityWeaver()
    : entity()
{
    mScale = 1.5;
    mRadius = 2.4;

    mScoreValue = 100;

    mType = ENTITY_TYPE_WEAVER;
    setState(ENTITY_STATE_INACTIVE);

    mAnimationIndex = 0;

    // 128, 255, 142
    mPen = vector::pen(.3, 1, .35, .5, 12);

    int i = 0;

    mModel.mNumVertex = 8;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, 1);
    mModel.mVertexList[i++] = Point3d(1, 0);
    mModel.mVertexList[i++] = Point3d(0, -1);
    mModel.mVertexList[i++] = Point3d(-1, 0);
    mModel.mVertexList[i++] = Point3d(-1, 1);
    mModel.mVertexList[i++] = Point3d(1, 1);
    mModel.mVertexList[i++] = Point3d(1, -1);
    mModel.mVertexList[i++] = Point3d(-1, -1);

    i = 0;

    mModel.mNumEdges = 8;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 7;
    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 4;

}

void entityWeaver::run()
{
    if (this->getEnabled())
    {
        // Check for missiles around us
        for (int i=0; i<entityPlayer1::mMaxMissiles; i++)
        {
            entityPlayerMissile* missile = &((entityPlayer1*)game::mPlayers.mPlayer1)->missiles[i];
            if (missile->getEnabled())
            {
                // Test this missile to see if it's aimed at us

                float angle = mathutils::calculate2dAngle(missile->getPos(), mPos);
                float missileAngle = mathutils::calculate2dAngle(Point3d(0,0,0), missile->getSpeed());

                float diff = mathutils::diffAngles(angle, missileAngle);

                if (fabs(diff) < 1)
                {
                    // And close to us
                    float distance = mathutils::calculate2dDistance(missile->getPos(), mPos);
                    if (distance < 25)
                    {
                        // Run away from it
                        float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.mPlayer1->getPos());
                        Point3d moveVector(.8, diff > 0 ? 1.1 : -1.1, 0);
                        moveVector = mathutils::rotate2dPoint(moveVector, angle + mathutils::DegreesToRads(180));
                        moveVector = mathutils::clamp2dVector(moveVector, .06);
                        mDrift += moveVector;
                    }
                }
            }
        }

        // Seek the player
        float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos());
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        moveVector = mathutils::clamp2dVector(moveVector, .05);
        mSpeed += moveVector;
        mSpeed = mathutils::clamp2dVector(mSpeed, .4 * mAggression);

        mSpeed *= .95;
    }

    entity::run();
}

void entityWeaver::spawnTransition()
{
    entity::spawnTransition();
    mRotationRate = -.06;
    game::mSound.playTrack(SOUNDID_ENEMYSPAWN3);
}



