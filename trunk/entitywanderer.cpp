#include "entitywanderer.h"
#include "game.h"


entityWanderer::entityWanderer()
    : entity()
{
   mScale = 1.7;
   mRadius = 2.6;

   mScoreValue = 25;

   mEdgeBounce = TRUE;

   mType = ENTITY_TYPE_WANDERER;
   setState(ENTITY_STATE_INACTIVE);

   mAnimationIndex = 0;

   // 211, 128, 255
   mPen = vector::pen(.65, .5, 1, .7, 12);

   int i = 0;

   mModel.mNumVertex = 9;
   mModel.mVertexList = new Point3d[mModel.mNumVertex];
   mModel.mVertexList[i++] = Point3d(0,0);
   mModel.mVertexList[i++] = Point3d(0,1);
   mModel.mVertexList[i++] = Point3d(1,1);
   mModel.mVertexList[i++] = Point3d(1,0);
   mModel.mVertexList[i++] = Point3d(1,-1);
   mModel.mVertexList[i++] = Point3d(0,-1);
   mModel.mVertexList[i++] = Point3d(-1,-1);
   mModel.mVertexList[i++] = Point3d(-1,0);
   mModel.mVertexList[i++] = Point3d(-1,1);

   mFlipped = (mathutils::frandFrom0To1() < .5);
   if (mFlipped)
   {
       for (int i=0; i<mModel.mNumVertex; i++)
       {
           float temp = mModel.mVertexList[i].y;
           mModel.mVertexList[i].y = mModel.mVertexList[i].x;
           mModel.mVertexList[i].x = temp;
       }
   }

   i = 0;

    mModel.mNumEdges = 12;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 7;
    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 8;
    mModel.mEdgeList[i].from = 8; mModel.mEdgeList[i++].to = 0;

}


void entityWanderer::run()
{
    if (this->getEnabled())
    {
        if ((mathutils::frandFrom0To1() * 40) < 1)
        {
            // Pick a random direction
            const float variation = 1.5;
            mCurrentHeading = mCurrentHeading + ((mathutils::frandFrom0To1() * variation) - (variation/2));
        }

        Point3d speed = Point3d(1,0,0);
        speed = mathutils::rotate2dPoint(speed, mCurrentHeading);
        speed *= .8;
        mSpeed += speed * .02;
        mSpeed = mathutils::clamp2dVector(mSpeed, .2); // No aggression
        mSpeed *= .98;

        // Change direction when we hit the grid edges
		if (game::mGrid.hitTest(mPos, getRadius()))
        {
            mCurrentHeading = mathutils::frandFrom0To1() * (2*PI);
        }
    }
    entity::run();
}

void entityWanderer::spawnTransition()
{
    entity::spawnTransition();

    mSpeed = Point3d(0,0,0);
    mAngle = 0;

    // Pick a random direction
    mCurrentHeading = mathutils::frandFrom0To1() * (2*PI);

    mAngle = 0;
    mRotationRate = mFlipped ? -.12 : .12;

    game::mSound.playTrack(SOUNDID_ENEMYSPAWN1);
}
