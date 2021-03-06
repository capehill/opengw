#include "entityline.h"
#include "game.h"

entityLine::entityLine(void)
{
    mScale = 1;
    mRadius = 1;

    mEdgeBounce = TRUE;
    mGridBound = TRUE;

    mTimeToLive = 0;
    mSpawnTime = 0;
    mDestroyTime = 0;

    mType = ENTITY_TYPE_LINE;
    setState(ENTITY_STATE_INACTIVE);

    mModel.mNumVertex = 2;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[0] = Point3d(0, 0);
    mModel.mVertexList[1] = Point3d(0, 0);

    mModel.mNumEdges = 1;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[0].from = 0; mModel.mEdgeList[0].to = 1;
}

void entityLine::spawnTransition()
{
    this->setState(entity::ENTITY_STATE_RUNNING);
    mTimeToLive = 100;
    run();
}

void entityLine::run()
{
    mPen.a *= .9;

    if (--mTimeToLive <= 0)
    {
        mTimeToLive = 0;
        this->setState(entity::ENTITY_STATE_DESTROYED);
    }

    mSpeed *= .985;

    entity::run();
}

void entityLine::addEndpoints(const Point3d& from, const Point3d& to)
{
    mModel.mVertexList[0] = Point3d(from);
    mModel.mVertexList[1] = Point3d(to);
}

