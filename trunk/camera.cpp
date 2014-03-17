#include "game.h"
#include "camera.h"

static const int zoomedIn = 50; // 50
static const int zoomedOut = 66;

camera::camera()
{
    center();
    mCurrentPos = mTargetPos;
}

void camera::center()
{
    mCurrentZoom = zoomedIn;
    mTargetZoom = zoomedIn;
    mTargetPos = Point3d((theGame.mGrid.extentX()-1)/2, (theGame.mGrid.extentY()-1)/2, mTargetZoom);
}

void camera::followPlayer()
{
    // If there is an active player or players, follow it
    Point3d playerPos;

    if (game::mNumPlayers == 1)
    {
		// One player game

        playerPos = game::mPlayers.mPlayer1->getPos();
    }
    else
    {
		// Two player game

        // Get the midpoint and distance between the players
        playerPos = Point3d((game::mPlayers.mPlayer1->getPos().x + game::mPlayers.mPlayer2->getPos().x) / 2, (game::mPlayers.mPlayer1->getPos().y + game::mPlayers.mPlayer2->getPos().y) / 2, 0);
        float distance = mathutils::calculate2dDistance(game::mPlayers.mPlayer1->getPos(), game::mPlayers.mPlayer2->getPos()) * 4;

        static const float hypotenuse = sqrt((float)(theGame.mGrid.extentX()*theGame.mGrid.extentX()) + (theGame.mGrid.extentY()*theGame.mGrid.extentY()));

        mTargetZoom = (zoomedIn + (zoomedOut-zoomedIn)) * (distance / hypotenuse);

        if (mTargetZoom < zoomedIn)
            mTargetZoom = zoomedIn;
        else if (mTargetZoom > zoomedOut)
            mTargetZoom = zoomedOut;
    }

    float ax = (playerPos.x / theGame.mGrid.extentX());
    float ay = (playerPos.y / theGame.mGrid.extentY());

    const int border = -20;

    mTargetPos.x = (ax * (theGame.mGrid.extentX() + (border*2))) - border;
    mTargetPos.y = (ay * (theGame.mGrid.extentY() + (border*2))) - border;
}

void camera::run()
{
    mCurrentZoom += (mTargetZoom - mCurrentZoom) / 20.0f;

    mCurrentPos.x += (mTargetPos.x - mCurrentPos.x) / 16.0f;
    mCurrentPos.y += (mTargetPos.y - mCurrentPos.y) / 16.0f;

    mCurrentPos.z = mCurrentZoom;
}

