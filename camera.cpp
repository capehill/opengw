#include "camera.hpp"
#include "game.hpp"
#include "players.hpp"

static const int zoomedIn = 46;  // 50
static const int zoomedOut = 72; // 66

camera::camera(const game& gameRef) : mGame(gameRef)
{
    center();
    mCurrentPos = mTargetPos;
}

void camera::center()
{
    mCurrentZoom = zoomedIn;
    mTargetZoom = zoomedIn;
    mTargetPos = Point3d((mGame.mGrid->extentX() - 1) / 2, (mGame.mGrid->extentY() - 1) / 2, mTargetZoom);
}

void camera::followPlayer()
{
    // If there is an active player or players, follow it
    Point3d playerPos;
    bool firstPlayer = true;
    int targets = 0;
    for (int i = 0; i < 4; i++) {
        player* player;
        switch (i) {
        case 0:
            player = mGame.mPlayers->mPlayer1;
            break;
        case 1:
            player = mGame.mPlayers->mPlayer2;
            break;
        case 2:
            player = mGame.mPlayers->mPlayer3;
            break;
        case 3:
            player = mGame.mPlayers->mPlayer4;
            break;
        }

        if (player->getState() != entity::ENTITY_STATE_INACTIVE) {
            ++targets;
            if (firstPlayer) {
                firstPlayer = false;
                playerPos = player->getPos();
            } else {
                playerPos.x = (playerPos.x + player->getPos().x) / 2;
                playerPos.y = (playerPos.y + player->getPos().y) / 2;
            }
        }
    }

    if (targets == 0) {
        // Nothing to follow
        return;
    }

    // Figure out our largest distance between players
    float playerDistance = 0;

    for (int i = 0; i < 4; i++) {
        player* playerA;
        switch (i) {
        case 0:
            playerA = mGame.mPlayers->mPlayer1;
            break;
        case 1:
            playerA = mGame.mPlayers->mPlayer2;
            break;
        case 2:
            playerA = mGame.mPlayers->mPlayer3;
            break;
        case 3:
            playerA = mGame.mPlayers->mPlayer4;
            break;
        }

        if (playerA->getState() != entity::ENTITY_STATE_INACTIVE) {
            for (int j = 0; j < 4; j++) {
                player* playerB;
                switch (j) {
                case 0:
                    playerB = mGame.mPlayers->mPlayer1;
                    break;
                case 1:
                    playerB = mGame.mPlayers->mPlayer2;
                    break;
                case 2:
                    playerB = mGame.mPlayers->mPlayer3;
                    break;
                case 3:
                    playerB = mGame.mPlayers->mPlayer4;
                    break;
                }

                if (playerA == playerB)
                    continue;
                if (playerB->getState() != entity::ENTITY_STATE_INACTIVE) {
                    float abDistance = mathutils::calculate2dDistance(playerA->getPos(), playerB->getPos());
                    if (abDistance > playerDistance)
                        playerDistance = abDistance;
                }
            }
        }
    }

    static const float hypotenuse = sqrt((float)(mGame.mGrid->extentX() * mGame.mGrid->extentX()) + (mGame.mGrid->extentY() * mGame.mGrid->extentY()));

    mTargetZoom = (zoomedIn + (zoomedOut - zoomedIn)) * ((playerDistance * 3) / hypotenuse);

    if (mTargetZoom < zoomedIn)
        mTargetZoom = zoomedIn;
    else if (mTargetZoom > zoomedOut)
        mTargetZoom = zoomedOut;

    float ax = (playerPos.x / mGame.mGrid->extentX());
    float ay = (playerPos.y / mGame.mGrid->extentY());

    const int border = -20;

    mTargetPos.x = (ax * (mGame.mGrid->extentX() + (border * 2))) - border;
    mTargetPos.y = (ay * (mGame.mGrid->extentY() + (border * 2))) - border;
}

void camera::run()
{
    mCurrentZoom += (mTargetZoom - mCurrentZoom) / 30.0f;

    mCurrentPos.x += (mTargetPos.x - mCurrentPos.x) / 30.0f;
    mCurrentPos.y += (mTargetPos.y - mCurrentPos.y) / 30.0f;

    mCurrentPos.z = mCurrentZoom;
}
