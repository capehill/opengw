#include "entitygeomsmall.h"

entityGeomSmall::entityGeomSmall()
    : entityGeom()
{
    mScale = .4;
    mRadius = .5;

    mScoreValue = 10;

    mType = ENTITY_TYPE_GEOM_SMALL;

    int i = 0;

    mModel.mNumVertex = 3;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, 1);
    mModel.mVertexList[i++] = Point3d(1, -1);
    mModel.mVertexList[i++] = Point3d(-1, -1);

    i = 0;

    mModel.mNumEdges = 3;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 0;
}
