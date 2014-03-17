#include "spawner.h"
#include "game.h"
#include <math.h>

#define MIN_SPAWN_DISTANCE 30
#define MAX_SPAWNLIST_ITEMS 50

spawner::spawner(void)
{
    clearWaveListEntities();
}

void spawner::init()
{
    mSpawnIndex = 0;
    mLastSpawnIndex = 0;

    mSpawnCheckTimer = 100;

    mSpawnWaitTimer = 50;

    mWaveIntervalTimer = 0;
    mWaveStartTimer = 0;
    mWaveEntityCounter = 0;
    mWaveEntity = entity::ENTITY_TYPE_UNDEF;
    mWaveType = WAVETYPE_SWARM;

    clearWaveListEntities();
    transition();
}

int spawner::getSpawnIndex() const
{
    return floor(mSpawnIndex);
}

void spawner::run(void)
{
    // Update our spawn index
//    if (getSpawnIndex() < MAX_SPAWNLIST_ITEMS)
    {
        mSpawnIndex += .002;
        if (mSpawnIndex > 15) mSpawnIndex = 15;
        if (getSpawnIndex() > mLastSpawnIndex)
        {
            mLastSpawnIndex = getSpawnIndex();
            transition();
        }
    }

/*
    // Keep things simple for the first level
    if (game::mLevel == 0)
    {
        if (mSpawnIndex > 1)
            mSpawnIndex = 1;
    }
*/

    // Update player 1 and player 2

    static int player1SpawnTimer = 0;
    static int player2SpawnTimer = 0;

    // Monitor Player1 and respawn as needed
    if (game::mPlayers.mPlayer1->getState() == entity::ENTITY_STATE_INACTIVE)
    {
        mSpawnWaitTimer = 50;

        if (++player1SpawnTimer >= 50)
        {
            player1SpawnTimer = 0;

            if (game::mPlayers.mPlayer1->getNumLives() > 0)
            {
                game::mPlayers.mPlayer1->takeLife();
                if (game::mPlayers.mPlayer1->getNumLives() > 0)
                {
                    game::mPlayers.mPlayer1->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
                }
                else
                {
                    // else game over
	    			game::mGameMode = game::GAMEMODE_GAMEOVER_TRANSITION;
                }
            }
            else
            {
                // else game over
	    		game::mGameMode = game::GAMEMODE_GAMEOVER_TRANSITION;
            }
        }
    }

    // Monitor Player2 and respawn as needed
    if (game::mNumPlayers == 2)
    {
        if (game::mPlayers.mPlayer2->getState() == entity::ENTITY_STATE_INACTIVE)
        {
            if (++player2SpawnTimer >= 50)
            {
                player2SpawnTimer = 0;

                if (game::mPlayers.mPlayer2->getNumLives() > 0)
                {
                    game::mPlayers.mPlayer2->takeLife();
                    if (game::mPlayers.mPlayer2->getNumLives() > 0)
                    {
                        game::mPlayers.mPlayer2->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
                    }
                    else
                    {
                        // else game over
    	    			game::mGameMode = game::GAMEMODE_GAMEOVER_TRANSITION;
                    }
                }
                else
                {
                    // else game over
    	    		game::mGameMode = game::GAMEMODE_GAMEOVER_TRANSITION;
                }
            }
        }
    }

/*
    if (game::mEnemies.getNumActiveEnemiesOfType(entity::ENTITY_TYPE_BLACKHOLE) == 0)
    {
        spawnEntities(entity::ENTITY_TYPE_BLACKHOLE, 1);
    }
return;
*/

    int numPlayersActive = 0;
    if (game::mPlayers.mPlayer1->getState() == entity::ENTITY_STATE_RUNNING)
        ++numPlayersActive;
    if (game::mNumPlayers == 2)
    {
        if (game::mPlayers.mPlayer2->getState() == entity::ENTITY_STATE_RUNNING)
            ++numPlayersActive;
    }

    if (mSpawnWaitTimer > 0) --mSpawnWaitTimer;

    if ((mSpawnWaitTimer == 0) && (numPlayersActive == game::mNumPlayers))
    {
        //
        // Randomly spawn enemies here and there
        //
        int index = getSpawnIndex();

#if 1
        if (++mSpawnCheckTimer > 100)
        {
            mSpawnCheckTimer = 0;

            // Wanderers
            if (index <= 1)
                spawnEntities(entity::ENTITY_TYPE_WANDERER, 2);
            else if (index > 1 && index < 3)
                spawnEntities(entity::ENTITY_TYPE_WANDERER, 4);

            // Grunts
            if (index <= 1)
            {
                spawnEntities(entity::ENTITY_TYPE_GRUNT, 2);
            }
            else if (index > 1)
            {
                spawnEntities(entity::ENTITY_TYPE_GRUNT, 4);
            }

            // Spinners
            if (index >= 1)
            {
                spawnEntities(entity::ENTITY_TYPE_SPINNER, 2);
            }

            // Weavers
            if (index >= 1)
            {
                spawnEntities(entity::ENTITY_TYPE_WEAVER, 2);
            }

            // Black holes
            if (index >= 4)
            {
                if (mathutils::frandFrom0To1() * 100 < 10)
                {
                    spawnEntities(entity::ENTITY_TYPE_BLACKHOLE, 1);
                }
            }
        }

		// DEBUG
/*
        if ((--mWaveStartTimer <= 0) && (mWaveEntity == entity::ENTITY_TYPE_UNDEF))
        {
            mWaveStartTimer = 500;
			mWaveEntity = entity::ENTITY_TYPE_SNAKE;
			mWaveType = WAVETYPE_RUSH;
			mWaveEntityCounter = (index >= 10) ? 12 : 6;
		}
*/
#endif

        runWaveListEntities();
        int numWaveEntities = numWaveListEntities();

        // Start a wave
        if (numWaveEntities <= 0)
        {
            if (index >= 2)
            {
                --mWaveStartTimer;
                if (mWaveStartTimer < 0) mWaveStartTimer=0;
                if ((mWaveStartTimer == 0) && (mWaveEntity == entity::ENTITY_TYPE_UNDEF))
                {
//                    mWaveStartTimer = 100;

                    switch ((int)(mathutils::frandFrom0To1() * 16))
                    {
                        default:
                        case 0:
                        case 1:
                        case 11:
                            mWaveEntity = entity::ENTITY_TYPE_GRUNT;
                            mWaveType = WAVETYPE_SWARM;
                            mWaveEntityCounter = (index >= 10) ? 60 : 40;
                            break;
                        case 2:
                        case 12:
                            mWaveEntity = entity::ENTITY_TYPE_WEAVER;
                            mWaveType = WAVETYPE_SWARM;
                            mWaveEntityCounter = (index >= 10) ? 24 : 12;
                            break;
                        case 3:
                        case 13:
                            mWaveEntity = entity::ENTITY_TYPE_SNAKE;
                            mWaveType = WAVETYPE_SWARM;
                            mWaveEntityCounter = (index >= 10) ? 16 : 8;
                            break;
                        case 4:
                        case 14:
                            mWaveEntity = entity::ENTITY_TYPE_SPINNER;
                            mWaveType = WAVETYPE_SWARM;
                            mWaveEntityCounter = (index >= 10) ? 16 : 8;
                            break;
                        case 10:
                        case 15:
                            mWaveEntity = entity::ENTITY_TYPE_BLACKHOLE;
                            mWaveType = WAVETYPE_SWARM;
                            mWaveEntityCounter = mathutils::frandFrom0To1() * 4;
                            break;

                        case 5:
                            mWaveEntity = entity::ENTITY_TYPE_GRUNT;
                            mWaveType = WAVETYPE_RUSH;
                            mWaveEntityCounter = 8;
                            break;
                        case 6:
                            mWaveEntity = entity::ENTITY_TYPE_WEAVER;
                            mWaveType = WAVETYPE_RUSH;
                            mWaveEntityCounter = 8;
                            break;
                        case 7:
                            mWaveEntity = entity::ENTITY_TYPE_SNAKE;
                            mWaveType = WAVETYPE_RUSH;
                            mWaveEntityCounter = (index >= 10) ? 8 : 4;
                            break;
                        case 8:
                            mWaveEntity = entity::ENTITY_TYPE_SPINNER;
                            mWaveType = WAVETYPE_RUSH;
                            mWaveEntityCounter = (index >= 10) ? 8 : 4;
                            break;

                        case 9:
                            if (index >= 8)
                            {
                                mWaveEntity = entity::ENTITY_TYPE_MAYFLY;
                                mWaveType = WAVETYPE_SWARM;
                                mWaveEntityCounter = 50;
                            }
                            break;
                    }
                }
            }
        }

        // Run waves
        if (mWaveEntity != entity::ENTITY_TYPE_UNDEF)
        {
            if (++mWaveIntervalTimer > 10)
            {
                mWaveIntervalTimer = 0;
                runWaves();
            }
        }
    }
    else
    {
        mSpawnCheckTimer = 0;
    }

    // Run the mayfly sound loop
    if (game::mEnemies.getNumActiveEnemiesOfType(entity::ENTITY_TYPE_MAYFLY) > 0)
    {
        if (!game::mSound.isTrackPlaying(SOUNDID_MAYFLIES))
            game::mSound.playTrack(SOUNDID_MAYFLIES);
    }
    else
    {
        if (game::mSound.isTrackPlaying(SOUNDID_MAYFLIES))
            game::mSound.stopTrack(SOUNDID_MAYFLIES);
    }

}

void spawner::transition()
{
//    TCHAR s[256];
//    wsprintf(s, L"New spawnIndex = %d\n", getSpawnIndex());
//    OutputDebugString(s);

//    int index = getSpawnIndex();

}


void spawner::spawnEntities(entity::EntityType type, int numWanted)
{
    const float margin = 15;
    const float leftEdge = margin;
    const float bottomEdge = margin;
    const float rightEdge = (theGame.mGrid.extentX()-1)-margin;
    const float topEdge = (theGame.mGrid.extentY()-1)-margin;


    int numHave = game::mEnemies.getNumActiveEnemiesOfType(type);
    if (numHave < numWanted)
    {
        entity* enemy = game::mEnemies.getUnusedEnemyOfType(type);
        if (enemy)
        {
            // Spawn somewhere random but not too close to the player
            float distance;
            float x = (mathutils::frandFrom0To1() * (rightEdge-leftEdge))+leftEdge;
            float y = (mathutils::frandFrom0To1() * (topEdge-bottomEdge))+bottomEdge;
            Point3d spawnPoint(x,y, 0);

	        Point3d hitPoint;
            if (game::mGrid.hitTest(spawnPoint, enemy->getRadius(), &hitPoint))
	        {
		        spawnPoint = hitPoint;
            }

            enemy->setPos(spawnPoint);

            // TODO - FIX THIS SO ENEMIES ARE SPAWNED OFF A RADIAL AT A MIN DISTANCE RATHER THAN
            // JUST USING RANDOM XY COORDS ON THE GRID?
            distance = mathutils::calculate2dDistance(spawnPoint, game::mPlayers.mPlayer1->getPos());

            enemy->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
        }
    }
}

void spawner::runWaves()
{
    if (mWaveEntity != entity::ENTITY_TYPE_UNDEF)
    {
        const float margin = 2;
        const float leftEdge = margin;
        const float bottomEdge = margin;
        const float rightEdge = (theGame.mGrid.extentX()-1)-margin;
        const float topEdge = (theGame.mGrid.extentY()-1)-margin;

        if (mWaveType == WAVETYPE_RUSH)
        {
            // Pick a player to attack
            Point3d playerPos = (game::mNumPlayers == 1) ? game::mPlayers.mPlayer1->getPos()
                : (mathutils::frandFrom0To1() * 100 < 50 ? game::mPlayers.mPlayer1->getPos() : game::mPlayers.mPlayer2->getPos());

            do
            {
                entity* enemy = game::mEnemies.getUnusedEnemyOfType(mWaveEntity);
                if (enemy)
                {
                    addToWaveList(enemy);

                    float rx = (mathutils::frandFrom0To1() * 4)-2;
                    float ry = (mathutils::frandFrom0To1() * 4)-2;

                    Point3d spawnPoint(30, 0, 0);
                    spawnPoint = mathutils::rotate2dPoint(spawnPoint, mathutils::frandFrom0To1() * (2*PI));
                    spawnPoint.x += playerPos.x + rx;
                    spawnPoint.y += playerPos.y + ry;

                    // Keep it on the grid
	                Point3d hitPoint;
	                if (game::mGrid.hitTest(spawnPoint, enemy->getRadius(), &hitPoint))
	                {
		                spawnPoint = hitPoint;
                    }

                    enemy->setPos(spawnPoint);
                    enemy->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
                }

                // Decrease the counter even if we didn't find an available entity
                --mWaveEntityCounter;
                if (mWaveEntityCounter <= 0)
                {
                    // End the wave
                    mWaveEntity = entity::ENTITY_TYPE_UNDEF;
                }
            }
            while (mWaveEntityCounter > 0);

        }
        else
        {
            static int corner = 0;

            for (int i=0; i<4; i++)
            {
                if (mWaveEntityCounter > 0)
                {
                    entity* enemy = game::mEnemies.getUnusedEnemyOfType(mWaveEntity);
                    if (enemy)
                    {
                        addToWaveList(enemy);

                        float rx = (mathutils::frandFrom0To1() * 10)-5;
                        float ry = (mathutils::frandFrom0To1() * 10)-5;

                        Point3d spawnPoint;
                        switch (corner%4)
                        {
                            case 0:
                                spawnPoint = Point3d(leftEdge+rx, topEdge+ry);
                                break;
                            case 1:
                                spawnPoint = Point3d(rightEdge+rx, topEdge+ry);
                                break;
                            case 2:
                                spawnPoint = Point3d(rightEdge+rx, bottomEdge+ry);
                                break;
                            case 3:
                                spawnPoint = Point3d(leftEdge+rx, bottomEdge+ry);
                                break;
                        }

                        {
                            float radius = enemy->getRadius();

                            if (mWaveEntity == entity::ENTITY_TYPE_BLACKHOLE)
                                radius = 20;

                            const float leftEdge = radius;
                            const float bottomEdge = radius;
                            const float rightEdge = (theGame.mGrid.extentX() - radius)-1;
                            const float topEdge = (theGame.mGrid.extentY() - radius)-1;

                            if (spawnPoint.x < leftEdge)
                            {
                                spawnPoint.x = leftEdge;
                            }
                            else if (spawnPoint.x > rightEdge)
                            {
                                spawnPoint.x = rightEdge;
                            }
                            if (spawnPoint.y < bottomEdge)
                            {
                                spawnPoint.y = bottomEdge;
                            }
                            else if (spawnPoint.y > topEdge)
                            {
                                spawnPoint.y = topEdge;
                            }
                        }

                        // Keep it on the grid
	                    Point3d hitPoint;
	                    if (game::mGrid.hitTest(spawnPoint, enemy->getRadius(), &hitPoint))
	                    {
		                    spawnPoint = hitPoint;
                        }

                        enemy->setPos(spawnPoint);
                        enemy->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
                    }

                    // Decrease the counter even if we didn't find an available entity
                    --mWaveEntityCounter;
                    if (mWaveEntityCounter <= 0)
                    {
                        // End the wave
                        mWaveEntity = entity::ENTITY_TYPE_UNDEF;
                    }
                }

                ++corner;
            }
        }
    }
}

void spawner::addToWaveList(entity* e)
{
    for (int i=0; i<NUM_WAVELIST; i++)
    {
        if (mWaveList[i].e == NULL)
        {
            mWaveList[i].e = e;
            mWaveList[i].genId = e->getGenId();
            return;
        }
    }
}

int spawner::numWaveListEntities()
{
    int count = 0;
    for (int i=0; i<NUM_WAVELIST; i++)
    {
        if (mWaveList[i].e && (mWaveList[i].e->getGenId() == mWaveList[i].genId))
        {
            ++count;
        }
    }
    return count;
}

void spawner::runWaveListEntities()
{
    for (int i=0; i<NUM_WAVELIST; i++)
    {
        if (mWaveList[i].e && (mWaveList[i].e->getGenId() != mWaveList[i].genId))
        {
            mWaveList[i].e = NULL;
            mWaveList[i].genId = -1;
        }
    }
}

void spawner::clearWaveListEntities()
{
    for (int i=0; i<NUM_WAVELIST; i++)
    {
        mWaveList[i].e = NULL;
        mWaveList[i].genId = -1;
    }
}


