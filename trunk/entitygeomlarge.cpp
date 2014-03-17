#include "entitygeomlarge.h"

entityGeomLarge::entityGeomLarge()
    : entityGeom()
{
    mScale = .4;
    mRadius = .5;

    mScoreValue = 50;

    mType = ENTITY_TYPE_GEOM_LARGE;

    int i = 0;

    mModel.mNumVertex = 12;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(-.5, -2);
    mModel.mVertexList[i++] = Point3d(-.5, -.5);
    mModel.mVertexList[i++] = Point3d(-2, -.5);
    mModel.mVertexList[i++] = Point3d(-2, .5);
    mModel.mVertexList[i++] = Point3d(-.5, .5);
    mModel.mVertexList[i++] = Point3d(-.5, 2);
    mModel.mVertexList[i++] = Point3d(.5, 2);
    mModel.mVertexList[i++] = Point3d(.5, .5);
    mModel.mVertexList[i++] = Point3d(2, .5);
    mModel.mVertexList[i++] = Point3d(2, -.5);
    mModel.mVertexList[i++] = Point3d(.5, -.5);
    mModel.mVertexList[i++] = Point3d(.5, -2);

    i = 0;

    mModel.mNumEdges = 12;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 7;
    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 8;
    mModel.mEdgeList[i].from = 8; mModel.mEdgeList[i++].to = 9;
    mModel.mEdgeList[i].from = 9; mModel.mEdgeList[i++].to = 10;
    mModel.mEdgeList[i].from = 10; mModel.mEdgeList[i++].to = 11;
    mModel.mEdgeList[i].from = 11; mModel.mEdgeList[i++].to = 0;
}

