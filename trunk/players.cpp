#include "players.h"
#include "game.h"

player* players::mPlayer1;
player* players::mPlayer2;

players::players()
{
    mPlayer1 = new entityPlayer1;
    mPlayer1->setState(entity::ENTITY_STATE_INACTIVE);

    mPlayer2 = new entityPlayer2;
    mPlayer2->setState(entity::ENTITY_STATE_INACTIVE);
}

players::~players()
{
    delete mPlayer1;
    delete mPlayer2;
}

void players::run()
{
    for (int i=0; i<2; i++)
    {
        player* currentPlayer = (i==0) ? mPlayer1 : mPlayer2;

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
        if (mPlayer1->getEnabled() && mPlayer2->getEnabled())
        {
            float distance1 = mathutils::calculate2dDistance(point, game::mPlayers.mPlayer1->getPos());
            float distance2 = mathutils::calculate2dDistance(point, game::mPlayers.mPlayer2->getPos());
            return (distance1 < distance2) ? mPlayer1 : mPlayer2;
        }
        else if (mPlayer2->getEnabled())
            return mPlayer2;
        else
            return mPlayer1;
    }
}
