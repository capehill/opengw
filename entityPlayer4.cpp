#include "entityPlayer4.hpp"
#include "game.hpp"
#include "grid.hpp"

entityPlayer4::entityPlayer4()
    : player()
{
    mPlayerAssignment = 3;

    mType = ENTITY_TYPE_PLAYER4;

    mScale = .1;
    mRadius = 2;

    mPen = vector::pen(.3, .8, .3, 1, 12);
    mExhaustPen = vector::pen(.3, .8, .3, 40, 5);
    mMissilesPen = vector::pen(.3, .8, .3, 40, 5);
    mFontPen = vector::pen(.3, .8, .3, 40, 5);

    int i = 0;

    mModel.mVertexList.resize(7);

    mModel.mVertexList[i++] = Point3d(0, 18);

    mModel.mVertexList[i++] = Point3d(18, -4);
    mModel.mVertexList[i++] = Point3d(2, -13);
    mModel.mVertexList[i++] = Point3d(12, -16);

    mModel.mVertexList[i++] = Point3d(-18, -4);
    mModel.mVertexList[i++] = Point3d(-2, -13);
    mModel.mVertexList[i++] = Point3d(-12, -16);

    i = 0;

    mModel.mEdgeList.resize(8);
    mModel.mEdgeList[i].from = 0;
    mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1;
    mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2;
    mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3;
    mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0;
    mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4;
    mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5;
    mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6;
    mModel.mEdgeList[i++].to = 0;
}

void entityPlayer4::initPlayerForGame()
{
    player::initPlayerForGame();

    if (theGame->numPlayers() > 1) {
        mPos.x = (theGame->mGrid->extentX() / 2) + 9;
        mPos.y = (theGame->mGrid->extentY() / 2) - 9;
        mPos.z = 0;
    }
}

void entityPlayer4::spawnTransition()
{
    player::spawnTransition();

    mPos.x = (theGame->mGrid->extentX() / 2) + 9;
    mPos.y = (theGame->mGrid->extentY() / 2) - 9;
    mPos.z = 0;
    mAngle = 0;
}
