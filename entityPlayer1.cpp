#include "entityPlayer1.h"
#include "game.h"
#include "entityplayermissile.h"

entityPlayer1::entityPlayer1(const game& gameRef)
    : player(), mGame(gameRef)
{
    mPlayerAssignment = 0;

    mType = ENTITY_TYPE_PLAYER1;

    mScale = 1.7;
    mRadius = 2;

    mPen = vector::pen(1, 1, 1, 1, 12);
    mExhaustPen = vector::pen(.96, 1, .68, 40, 5);
    mMissilesPen = vector::pen(.96, 1, .68, 40, 5);
    mFontPen = vector::pen(1, 1, 0, 40, 5);

    mPos.x = mGame.mGrid.extentX() / 2;
    mPos.y = mGame.mGrid.extentY() / 2;
    mPos.z = 0;

    int i=0;

    mModel.mNumVertex = 8;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, -1);
    mModel.mVertexList[i++] = Point3d(1, -.15);
    mModel.mVertexList[i++] = Point3d(.5, .7);
    mModel.mVertexList[i++] = Point3d(.72, .02);
    mModel.mVertexList[i++] = Point3d(0, -.4);
    mModel.mVertexList[i++] = Point3d(-.72, .02);
    mModel.mVertexList[i++] = Point3d(-.5, .7);
    mModel.mVertexList[i++] = Point3d(-1, -.15);

    i = 0;

    mModel.mNumEdges = 8;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 7;
    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 0;
}

void entityPlayer1::initPlayerForGame()
{
    player::initPlayerForGame();

    if ((mGame.numPlayers() == 1) || (mGame.mGameMode != game::GAMEMODE_PLAYING))
    {
        mPen = vector::pen(1, 1, 1, 1, 12);
        mExhaustPen = vector::pen(.9, 1, .35, 1, 5);
        mMissilesPen = vector::pen(.9, 1, .35, 1, 5);
        mFontPen = vector::pen(1, 1, 0, 1, 5);

        mPos.x = mGame.mGrid.extentX() / 2;
        mPos.y = mGame.mGrid.extentY() / 2;
        mPos.z = 0;
    }
    else
    {
        mPen = vector::pen(1, .4, .4, 1, 12);
        mExhaustPen = vector::pen(1, .4, .4, 1, 5);
        mMissilesPen = vector::pen(1, .4, .4, 1, 5);
        mFontPen = vector::pen(1, .4, .4, 1, 5);

        mPos.x = (theGame->mGrid.extentX() / 2) - 9;
        mPos.y = (theGame->mGrid.extentY() / 2) + 9;
        mPos.z = 0;
    }

}

void entityPlayer1::spawnTransition()
{
    player::spawnTransition();

    if (theGame->numPlayers() > 1)
    {
        mPen = vector::pen(1, .4, .4, 1, 12);
        mExhaustPen = vector::pen(1, .4, .4, 1, 5);
        mMissilesPen = vector::pen(1, .4, .4, 1, 5);
        mFontPen = vector::pen(1, .4, .4, 40, 5);

        mPos.x = (mGame.mGrid.extentX() / 2) - 9;
        mPos.y = (mGame.mGrid.extentY() / 2) + 9;
        mPos.z = 0;
        mAngle = 0;
    }
}


