#include "entitygeommedium.h"

entityGeomMedium::entityGeomMedium()
    : entityGeom()
{
    mScale = .45;
    mRadius = .5;

    mScoreValue = 20;

    mType = ENTITY_TYPE_GEOM_MEDIUM;

    int i = 0;

    mModel.mNumVertex = 4;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(-2, 0);
    mModel.mVertexList[i++] = Point3d(0, 1);
    mModel.mVertexList[i++] = Point3d(2, 0);
    mModel.mVertexList[i++] = Point3d(0, -1);

    i = 0;

    mModel.mNumEdges = 4;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 0;
}

