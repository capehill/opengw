#include "players.h"
#include "game.h"

player* players::mPlayer1;
player* players::mPlayer2;
player* players::mPlayer3;
player* players::mPlayer4;

players::players()
{
    mPlayer1 = new entityPlayer1;
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
    for (int i=0; i<4; i++)
    {
        player* currentPlayer;
        
        switch (i)
        {
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

        switch(currentPlayer->getState())
        {
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
                    entity* enemy = game::mEnemies.hitTestEnemiesAtPosition(currentPlayer->getPos(), currentPlayer->getRadius()*.75);
                    if (enemy)
                    {
                        enemy = enemy->getParent();

                        if (currentPlayer->shields())
                        {
                            // We hit an enemy with shields still on
                            enemy->hit(currentPlayer);
                        }
                        else
                        {
                            // We hit an enemy

                            // Destroy the player
                            currentPlayer->setState(entity::ENTITY_STATE_DESTROY_TRANSITION);

                            // Set the enemy we hit to "indicate"
                            enemy->setState(entity::ENTITY_STATE_INDICATE_TRANSITION);
                            enemy->incGenId();

                            // Destroy all the other enemies
                            if ((game::mNumPlayers == 1) || (game::mNumPlayers == 2 && game::m2PlayerNumLives <= 1))
                            {
                                for (int i=0; i<NUM_ENEMIES; i++)
                                {
                                    if ((game::mEnemies.mEnemies[i]->getState() != entity::ENTITY_STATE_INACTIVE) && (game::mEnemies.mEnemies[i] != enemy))
                                    {
                                        game::mEnemies.mEnemies[i]->hit(NULL);
                                        game::mEnemies.mEnemies[i]->incGenId();
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
    if (game::mNumPlayers == 1)
    {
        return mPlayer1;
    }
    else
    {
        float distancePlayer1 = 999999;
        float distancePlayer2 = 999999;
        float distancePlayer3 = 999999;
        float distancePlayer4 = 999999;

        if (mPlayer1->getEnabled())
        {
            distancePlayer1 = mathutils::calculate2dDistance(point, game::mPlayers.mPlayer1->getPos());
        }
        if (mPlayer2->getEnabled())
        {
            distancePlayer2 = mathutils::calculate2dDistance(point, game::mPlayers.mPlayer2->getPos());
        }
        if (mPlayer3->getEnabled())
        {
            distancePlayer3 = mathutils::calculate2dDistance(point, game::mPlayers.mPlayer3->getPos());
        }
        if (mPlayer4->getEnabled())
        {
            distancePlayer4 = mathutils::calculate2dDistance(point, game::mPlayers.mPlayer4->getPos());
        }

        float minDistance = 999999;
        player* closePlayer = NULL;

        if (distancePlayer1 < minDistance)
        {
            minDistance = distancePlayer1;
            closePlayer = game::mPlayers.mPlayer1;
        }
        if (distancePlayer2 < minDistance)
        {
            minDistance = distancePlayer2;
            closePlayer = game::mPlayers.mPlayer2;
        }
        if (distancePlayer3 < minDistance)
        {
            minDistance = distancePlayer3;
            closePlayer = game::mPlayers.mPlayer3;
        }
        if (distancePlayer4 < minDistance)
        {
            minDistance = distancePlayer4;
            closePlayer = game::mPlayers.mPlayer4;
        }

        return closePlayer ? closePlayer : mPlayer1; // just in case

    }
}
