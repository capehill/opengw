#include "players.hpp"
#include "enemies.hpp"
#include "entityPlayer1.hpp"
#include "entityPlayer2.hpp"
#include "entityPlayer3.hpp"
#include "entityPlayer4.hpp"
#include "game.hpp"

player* players::mPlayer1;
player* players::mPlayer2;
player* players::mPlayer3;
player* players::mPlayer4;

players::players(const game& gameRef) : mGame(gameRef)
{
    mPlayer1 = new entityPlayer1(gameRef);
    mPlayer1->setState(entity::ENTITY_STATE_INACTIVE);

    mPlayer2 = new entityPlayer2;
    mPlayer2->setState(entity::ENTITY_STATE_INACTIVE);

    mPlayer3 = new entityPlayer3;
    mPlayer3->setState(entity::ENTITY_STATE_INACTIVE);

    mPlayer4 = new entityPlayer4;
    mPlayer4->setState(entity::ENTITY_STATE_INACTIVE);
}

players::~players()
{
    delete mPlayer1;
    delete mPlayer2;
    delete mPlayer3;
    delete mPlayer4;
}

void players::run()
{
    for (int i = 0; i < 4; i++) {
        player* currentPlayer;

        switch (i) {
        case 0:
            currentPlayer = mPlayer1;
            break;
        case 1:
            currentPlayer = mPlayer2;
            break;
        case 2:
            currentPlayer = mPlayer3;
            break;
        case 3:
            currentPlayer = mPlayer4;
            break;
        }

        currentPlayer->runMissiles();

        switch (currentPlayer->getState()) {
        case entity::ENTITY_STATE_INACTIVE:
            continue;
        case entity::ENTITY_STATE_SPAWN_TRANSITION:
            currentPlayer->spawnTransition();
            break;
        case entity::ENTITY_STATE_SPAWNING:
            currentPlayer->spawn();
            break;
        case entity::ENTITY_STATE_RUN_TRANSITION:
            currentPlayer->runTransition();
            break;
        case entity::ENTITY_STATE_RUNNING:
            currentPlayer->run();

            // Hit test the players against the enemies
            //---------------------------------------------------------------------------------------
            if (1) // Change to 0 to cheat for testing :-)
            {
                entity* enemy = theGame->mEnemies->hitTestEnemiesAtPosition(currentPlayer->getPos(), currentPlayer->getRadius() * .75);
                if (enemy) {
                    enemy = enemy->getParent();

                    if (currentPlayer->shields()) {
                        // We hit an enemy with shields still on
                        enemy->hit(currentPlayer);
                    } else {
                        // We hit an enemy

                        // Destroy the player
                        currentPlayer->setState(entity::ENTITY_STATE_DESTROY_TRANSITION);

                        // Set the enemy we hit to "indicate"
                        enemy->setState(entity::ENTITY_STATE_INDICATE_TRANSITION);
                        enemy->incGenId();

                        // Destroy all the other enemies
                        if (mGame.numPlayers() == 1) {
                            for (int i = 0; i < NUM_ENEMIES; i++) {
                                if ((theGame->mEnemies->mEnemies[i]->getState() != entity::ENTITY_STATE_INACTIVE) && (theGame->mEnemies->mEnemies[i] != enemy)) {
                                    theGame->mEnemies->mEnemies[i]->hit(NULL);
                                    theGame->mEnemies->mEnemies[i]->incGenId();
                                }
                            }
                        }
                    }
                }
            }
            //---------------------------------------------------------------------------------------

            break;
        case entity::ENTITY_STATE_DESTROY_TRANSITION:
            currentPlayer->destroyTransition();
            break;
        case entity::ENTITY_STATE_DESTROYED:
            currentPlayer->destroy();
            break;
        default:
            printf("Unhandled player state\n");
            break;
        }
    }
}

void players::draw()
{
    mPlayer1->draw();
    mPlayer2->draw();
    mPlayer3->draw();
    mPlayer4->draw();
}

player* players::getPlayerClosestToPosition(const Point3d& point)
{
    // The enemies use this function to determine which player to chase (mostly in the case of a non-single player game) :-)
    if (mGame.numPlayers() == 1) {
        return mPlayer1;
    } else {
        float distancePlayer1 = 999999;
        float distancePlayer2 = 999999;
        float distancePlayer3 = 999999;
        float distancePlayer4 = 999999;

        if (mPlayer1->getEnabled()) {
            distancePlayer1 = mathutils::calculate2dDistance(point, mGame.mPlayers->mPlayer1->getPos());
        }
        if (mPlayer2->getEnabled()) {
            distancePlayer2 = mathutils::calculate2dDistance(point, mGame.mPlayers->mPlayer2->getPos());
        }
        if (mPlayer3->getEnabled()) {
            distancePlayer3 = mathutils::calculate2dDistance(point, mGame.mPlayers->mPlayer3->getPos());
        }
        if (mPlayer4->getEnabled()) {
            distancePlayer4 = mathutils::calculate2dDistance(point, mGame.mPlayers->mPlayer4->getPos());
        }

        float minDistance = 999999;
        player* closePlayer = NULL;

        if (distancePlayer1 < minDistance) {
            minDistance = distancePlayer1;
            closePlayer = mGame.mPlayers->mPlayer1;
        }
        if (distancePlayer2 < minDistance) {
            minDistance = distancePlayer2;
            closePlayer = mGame.mPlayers->mPlayer2;
        }
        if (distancePlayer3 < minDistance) {
            minDistance = distancePlayer3;
            closePlayer = mGame.mPlayers->mPlayer3;
        }
        if (distancePlayer4 < minDistance) {
            // minDistance = distancePlayer4;
            closePlayer = mGame.mPlayers->mPlayer4;
        }

        return closePlayer ? closePlayer : mPlayer1; // just in case
    }
}

player* players::getRandomActivePlayer()
{
    bool p1, p2, p3, p4;

    p1 = false;
    p2 = false;
    p3 = false;
    p4 = false;

    if (mPlayer1->mJoined) {
        if (mPlayer1->getState() == entity::ENTITY_STATE_RUNNING)
            p1 = true;
    }
    if (mPlayer2->mJoined) {
        if (mPlayer2->getState() == entity::ENTITY_STATE_RUNNING)
            p2 = true;
    }
    if (mPlayer3->mJoined) {
        if (mPlayer3->getState() == entity::ENTITY_STATE_RUNNING)
            p3 = true;
    }
    if (mPlayer4->mJoined) {
        if (mPlayer4->getState() == entity::ENTITY_STATE_RUNNING)
            p4 = true;
    }

    if (!p1 && !p2 && !p3 && !p4) {
        // Nobody's home
        return NULL;
    }
    if (p1 && !p2 && !p3 && !p4) {
        return mPlayer1;
    }
    if (!p1 && p2 && !p3 && !p4) {
        return mPlayer2;
    }
    if (!p1 && !p2 && p3 && !p4) {
        return mPlayer3;
    }
    if (!p1 && !p2 && !p3 && p4) {
        return mPlayer4;
    } else {
        while (1) {
            int player = ceil(mathutils::frandFrom0To1() * 4);
            if ((player == 0) && p1) {
                return mPlayer1;
            }
            if ((player == 1) && p2) {
                return mPlayer2;
            }
            if ((player == 2) && p3) {
                return mPlayer3;
            }
            if ((player == 3) && p4) {
                return mPlayer4;
            }
        }
    }

    return NULL;
}
