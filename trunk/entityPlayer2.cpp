#include "entityPlayer2.h"
#include "game.h"
#include "entityPlayerMissile.h"


entityPlayer2::entityPlayer2()
    : player()
{
    mPlayerAssignment = 1;

    mType = ENTITY_TYPE_PLAYER2;

    mScale = .1;
    mRadius = 2;

    mScoreValue = 0;

    mSpawnTime = 35;
    mSheildTimer = PLAYER_SHEILD_TIME;

    mPen = vector::pen(.3, .4, 1, 1, 12);
    mExhaustPen = vector::pen(.3, .4, 1, 40, 5);
    mMissilesPen = vector::pen(.3, .4, 1, 40, 5);
    mFontPen = vector::pen(.3, .4, 1, 40, 5);

    int i=0;

    mModel.mNumVertex = 13;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, 17.5);
    mModel.mVertexList[i++] = Point3d(6.5, 5.5);
    mModel.mVertexList[i++] = Point3d(0, -6.5);
    mModel.mVertexList[i++] = Point3d(-6.5, 5.5);

    mModel.mVertexList[i++] = Point3d(1.5, -10.5);
    mModel.mVertexList[i++] = Point3d(15, 1.5);
    mModel.mVertexList[i++] = Point3d(9.2, -13.1);

    mModel.mVertexList[i++] = Point3d(-1.5, -10.5);
    mModel.mVertexList[i++] = Point3d(-15, 1.5);
    mModel.mVertexList[i++] = Point3d(-9.2, -13.1);

    mModel.mVertexList[i++] = Point3d(5, -17.7);
    mModel.mVertexList[i++] = Point3d(0, -14.5);
    mModel.mVertexList[i++] = Point3d(-5, -17.7);

    i = 0;

    mModel.mNumEdges = 12;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 0;

    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 4;

    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 9;
    mModel.mEdgeList[i].from = 9; mModel.mEdgeList[i++].to = 8;
    mModel.mEdgeList[i].from = 8; mModel.mEdgeList[i++].to = 7;

    mModel.mEdgeList[i].from = 10; mModel.mEdgeList[i++].to = 11;
    mModel.mEdgeList[i].from = 11; mModel.mEdgeList[i++].to = 12;
}

void entityPlayer2::initPlayerForGame()
{
    player::initPlayerForGame();

    if (game::mNumPlayers == 2)
    {
        mPos.x = (theGame.mGrid.extentX() / 2) + 9;
        mPos.y = (theGame.mGrid.extentY() / 2) + 9;
        mPos.z = 0;
    }
}

void entityPlayer2::spawnTransition()
{
    player::spawnTransition();

    mPos.x = (theGame.mGrid.extentX() / 2) + 9;
    mPos.y = (theGame.mGrid.extentY() / 2) + 9;
    mPos.z = 0;
    mAngle = 0;
}
