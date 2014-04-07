#include "entityPlayer3.h"
#include "game.h"
#include "entityPlayerMissile.h"


entityPlayer3::entityPlayer3()
    : player()
{
    mPlayerAssignment = 2;

    mType = ENTITY_TYPE_PLAYER3;

    mScale = .1;
    mRadius = 2;

    mScoreValue = 0;

    mSpawnTime = 35;
    mSheildTimer = PLAYER_SHEILD_TIME;

    mPen = vector::pen(1, 1, .3, 1, 12);
    mExhaustPen = vector::pen(1, 1, .5, 40, 5);

    int i=0;

    mModel.mNumVertex = 12;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];

    mModel.mVertexList[i++] = Point3d(0, 15);
    mModel.mVertexList[i++] = Point3d(5, -2);
    mModel.mVertexList[i++] = Point3d(0, -11);
    mModel.mVertexList[i++] = Point3d(-5, -2);

    mModel.mVertexList[i++] = Point3d(11, 9);
    mModel.mVertexList[i++] = Point3d(18, -2);
    mModel.mVertexList[i++] = Point3d(6, -11);
    mModel.mVertexList[i++] = Point3d(12, -2);

    mModel.mVertexList[i++] = Point3d(-11, 9);
    mModel.mVertexList[i++] = Point3d(-18, -2);
    mModel.mVertexList[i++] = Point3d(-6, -11);
    mModel.mVertexList[i++] = Point3d(-12, -2);

    i = 0;

    mModel.mNumEdges = 12;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 0;

    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 7;

    mModel.mEdgeList[i].from = 11; mModel.mEdgeList[i++].to = 8;
    mModel.mEdgeList[i].from = 8; mModel.mEdgeList[i++].to = 9;
    mModel.mEdgeList[i].from = 9; mModel.mEdgeList[i++].to = 10;
    mModel.mEdgeList[i].from = 10; mModel.mEdgeList[i++].to = 11;
}

void entityPlayer3::initPlayerForGame()
{
    player::initPlayerForGame();

    if (game::mNumPlayers > 1)
    {
        mPos.x = (theGame.mGrid.extentX() / 2) - 9;
        mPos.y = (theGame.mGrid.extentY() / 2) - 9;
        mPos.z = 0;
    }
}

void entityPlayer3::spawnTransition()
{
    player::spawnTransition();

    mPos.x = (theGame.mGrid.extentX() / 2) - 9;
    mPos.y = (theGame.mGrid.extentY() / 2) - 9;
    mPos.z = 0;
    mAngle = 0;
}
