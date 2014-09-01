#include "spawner.h"
#include "game.h"
#include <math.h>

#define MIN_SPAWN_DISTANCE 30
#define MAX_SPAWN_INDEX 40

spawner::spawner(void)
{
}

void spawner::init()
{
    mNumWavesAllowed = 0;
    mSpawnIndex = 0;
    mLastSpawnIndex = 0;

    mSpawnCheckTimer = 100;
    mSpawnWaitTimer = 50;
    mWaveStartTimer = 0;

    clearWaveData();
    transition();
}

int spawner::getSpawnIndex() const
{
    return floor(mSpawnIndex);
}

void spawner::run(void)
{
    // Update our spawn index
    if (getSpawnIndex() < MAX_SPAWN_INDEX)
    {
        mSpawnIndex += .0008;
        if (mSpawnIndex > MAX_SPAWN_INDEX) mSpawnIndex = MAX_SPAWN_INDEX;
        if (getSpawnIndex() > mLastSpawnIndex)
        {
            mLastSpawnIndex = getSpawnIndex();
            mSpawnProgress = mSpawnIndex / MAX_SPAWN_INDEX;
            if (mSpawnProgress > 1) mSpawnProgress = 1;
            transition();
        }
    }

    int numPlayersActive = 0;
    if (game::mPlayers.mPlayer1->mJoined)
    {
        if (game::mPlayers.mPlayer1->getState() == entity::ENTITY_STATE_RUNNING)
            ++numPlayersActive;
    }
    if (game::mPlayers.mPlayer2->mJoined)
    {
        if (game::mPlayers.mPlayer2->getState() == entity::ENTITY_STATE_RUNNING)
            ++numPlayersActive;
    }
    if (game::mPlayers.mPlayer3->mJoined)
    {
        if (game::mPlayers.mPlayer3->getState() == entity::ENTITY_STATE_RUNNING)
            ++numPlayersActive;
    }
    if (game::mPlayers.mPlayer4->mJoined)
    {
        if (game::mPlayers.mPlayer4->getState() == entity::ENTITY_STATE_RUNNING)
            ++numPlayersActive;
    }

    // Respawn the players as needed

    static int player1SpawnTimer = 0;
    static int player2SpawnTimer = 0;
    static int player3SpawnTimer = 0;
    static int player4SpawnTimer = 0;

    // Monitor players and respawn as needed
    for (int i = 0; i<4; i++)
    {
        player* player;
        int* timer;
        switch(i)
        {
            case 0:
                player = game::mPlayers.mPlayer1;
                timer = &player1SpawnTimer;
                break;
            case 1:
                player = game::mPlayers.mPlayer2;
                timer = &player2SpawnTimer;
                break;
            case 2:
                player = game::mPlayers.mPlayer3;
                timer = &player3SpawnTimer;
                break;
            case 3:
                player = game::mPlayers.mPlayer4;
                timer = &player4SpawnTimer;
                break;
        }

        if (player->mJoined && (player->getState() == entity::ENTITY_STATE_INACTIVE))
        {
            if (theGame.numPlayers() == 1)
            {
                mSpawnWaitTimer = 50;
            }

            if (++(*timer) >= 50)
            {
                *timer = 0;

                player->takeLife();
                if (player->getNumLives() > 0)
                {
                    player->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
                }
                else
                {
                    if (theGame.numPlayers() == 1)
                    {
	    			    game::mGameMode = game::GAMEMODE_GAMEOVER_TRANSITION;
                    }
                    else
                    {
                        // Multiplayer
                        // Wait until no more players are active
                        if (numPlayersActive == 0)
                        {
    	    			    game::mGameMode = game::GAMEMODE_GAMEOVER_TRANSITION;
                        }
                    }
                }
            }
        }
    }

    if (mSpawnWaitTimer > 0) --mSpawnWaitTimer;

    if (!numPlayersActive)
    {
        mWaveStartTimer = 0;
    }


/*
    if (numWaveData() == 0)
    {
        newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_BLACKHOLE, 1);
    }
    runWaves();
    return;
*/



    if ((mSpawnWaitTimer <= 0) && numPlayersActive && !game::mBomb.isBombing())
    {
        //
        // Randomly spawn enemies here and there
        //
        int index = getSpawnIndex();

        if ((++mSpawnCheckTimer > 100) || (index > 10))
        {
            mSpawnCheckTimer = 0;

            // Wanderers
            if (index <= 1)
                spawnEntities(entity::ENTITY_TYPE_WANDERER, 2);
            else if (index > 1 && index < 3)
                spawnEntities(entity::ENTITY_TYPE_WANDERER, 4);

            // Grunts
            spawnEntities(entity::ENTITY_TYPE_GRUNT, (index <= 1) ? 2 : 4);

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
            if (index >= 2)
            {
                if (mathutils::frandFrom0To1() * 100 < 4)
                {
                    spawnEntities(entity::ENTITY_TYPE_BLACKHOLE, 1);
                }
            }
/*
            if (game::mEnemies.getNumActiveEnemiesOfType(entity::ENTITY_TYPE_BLACKHOLE) == 0)
            {
                spawnEntities(entity::ENTITY_TYPE_BLACKHOLE, 1);
            }
*/
        }

        mNumWavesAllowed = 0;
        if (index > 1)
        {
            mNumWavesAllowed = 1;
        }
        if (index > 12)
        {
            mNumWavesAllowed = 2;
        }
        if (index > 20)
        {
            // Make it rain!!
            mNumWavesAllowed = 9999;
        }

        if ((++mWaveStartTimer >= 20) && (numWaveData() < mNumWavesAllowed))
        {
            mWaveStartTimer = 0;

            switch ((int)(mathutils::frandFrom0To1() * 13))
            {
                //
                // SWARM TYPE
                //
                case 0:
                    newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_GRUNT, max(20, ceil(numEnemyGrunt * mSpawnProgress)));
                    break;
                case 1:
                    newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_WEAVER, max(20, ceil(numEnemyWeaver * mSpawnProgress)));
                    break;
                case 2:
                    if (index > 4)
                    {
                        newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_SNAKE, max(8, ceil(numEnemySnake * mSpawnProgress)));
                    }
                    break;
                case 3:
                    newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_SPINNER, max(20, ceil(numEnemySpinner * mSpawnProgress)));
                    break;
                case 4:
                    if (index > 4)
                    {
                        newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_BLACKHOLE, max(4, ceil(mathutils::frandFrom0To1() * numEnemyBlackHole * mSpawnProgress)));
                    }
                    break;
                case 5:
                    if (index > 8)
                    {
                        newWave(WAVETYPE_SWARM, entity::ENTITY_TYPE_MAYFLY, max(50, ceil(numEnemyMayfly * mSpawnProgress)));
                    }
                    break;
                //
                // RUSH TYPE
                //
                case 6:
                    newWave(WAVETYPE_RUSH, entity::ENTITY_TYPE_GRUNT, max(20, ceil(numEnemyGrunt * mSpawnProgress) / 2));
                    break;
                case 7:
                    newWave(WAVETYPE_RUSH, entity::ENTITY_TYPE_WEAVER, max(20, ceil(numEnemyWeaver * mSpawnProgress) / 2));
                    break;
                case 8:
                    if (index > 4)
                    {
                        newWave(WAVETYPE_RUSH, entity::ENTITY_TYPE_SNAKE, max(8, ceil(numEnemySnake * mSpawnProgress) / 2));
                    }
                    break;
                case 9:
                    newWave(WAVETYPE_RUSH, entity::ENTITY_TYPE_SPINNER, max(20, ceil(numEnemySpinner * mSpawnProgress) / 2));
                    break;
                case 10:
//                    newWave(WAVETYPE_RUSH, entity::ENTITY_TYPE_BLACKHOLE, ceil(mathutils::frandFrom0To1() * numEnemyBlackHole * mSpawnProgress) / 2);
                    break;
                case 11:
                    if (index > 4)
                    {
                        newWave(WAVETYPE_RUSH, entity::ENTITY_TYPE_REPULSOR, ceil(mathutils::frandFrom0To1() * numEnemyRepulsor * mSpawnProgress) / 2);
                    }
                    break;
            }
        }

        runWaves();
    }
    else
    {
        mSpawnCheckTimer = 0;
    }

    // Run the mayfly sound loop
    // This is a dumb place for this to live
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

// Gets called every time we go from one spawn index to another
// Not currently used
void spawner::transition()
{
    char s[256];
    sprintf(s, "New spawnIndex = %d, amount=%f\n", getSpawnIndex(), mSpawnProgress);
    OutputDebugStringA(s);
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
    for (int i=0; i<NUM_WAVEDATA; i++)
    {
        WAVEDATA* wd = &mWaveData[i];
        if (wd->mWaveType != WAVETYPE_UNUSED)
        {
            static const float margin = 2;
            static const float leftEdge = margin;
            static const float bottomEdge = margin;
            static const float rightEdge = (theGame.mGrid.extentX()-1)-margin;
            static const float topEdge = (theGame.mGrid.extentY()-1)-margin;

            if (wd->mWaveType == WAVETYPE_RUSH)
            {
                // Pick a player to attack
                player* randPlayer = game::mPlayers.getRandomActivePlayer();
                if (!randPlayer) return;
                Point3d playerPos = randPlayer->getPos();

                // Keep cranking out enemies until the spawn counter reaches 0
                while (wd->spawnCount-- > 0)
                {
                    entity* enemy = game::mEnemies.getUnusedEnemyOfType(wd->entityType);
                    if (enemy)
                    {
                        // Add the enemy to our tracking list
                        addEntityToWaveTracker(wd, enemy);

                        float rx = (mathutils::frandFrom0To1() * 4)-2;
                        float ry = (mathutils::frandFrom0To1() * 4)-2;

                        Point3d spawnPoint(wd->entityType == entity::ENTITY_TYPE_BLACKHOLE ? 80 : 40, 0, 0);
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
                };

            }
            else
            {
                if (wd->timer == 0)
                {
                    static int corner = 0;

                    for (int n=0; n<4; n++)
                    {
                        // Keep cranking out enemies until the spawn counter reaches 0
                        if (wd->spawnCount-- > 0)
                        {
                            entity* enemy = game::mEnemies.getUnusedEnemyOfType(wd->entityType);
                            if (enemy)
                            {
                                // Add the enemy to our tracking list
                                addEntityToWaveTracker(wd, enemy);

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

                                    if (wd->entityType == entity::ENTITY_TYPE_BLACKHOLE)
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
                        }

                        ++corner;
                    }
                }

                ++wd->timer;

                if (wd->timer > 10)
                {
                    wd->timer = 0;
                }

            }
        }

        if (wd->spawnCount <= 0)
        {
            // Keep tracking previously spawned entities so we know when the wave is completed
            bool running = false;
            for (int n=0; n<NUM_WAVEITEMTRACKERS; n++)
            {
                if (wd->mItemTrackers[n].e && (wd->mItemTrackers[n].e->getGenId() == wd->mItemTrackers[n].genId))
                {
                    running = true;
                    break;
                }
            }
            if (!running)
            {
                // No more entities from this wave exist - end the wave
                wd->mWaveType = WAVETYPE_UNUSED;
            }
        }
    }
}

void spawner::clearWaveData()
{
    for (int i=0; i<NUM_WAVEDATA; i++)
    {
        WAVEDATA* wd = &mWaveData[i];

        wd->mWaveType = WAVETYPE_UNUSED;

        for (int n=0; n<NUM_WAVEITEMTRACKERS; n++)
        {
            wd->mItemTrackers[n].e = NULL;
            wd->mItemTrackers[n].genId = -1;
        }
    }
}

void spawner::newWave(WAVETYPE mWaveType, entity::EntityType entityType, int spawnCount)
{
    WAVEDATA* wd = getUnusedWaveData();
    if (wd)
    {
        wd->mWaveType = mWaveType;
        wd->entityType = entityType;
        wd->spawnCount = spawnCount;
        wd->timer = 0;

        for (int n=0; n<NUM_WAVEITEMTRACKERS; n++)
        {
            wd->mItemTrackers[n].e = NULL;
            wd->mItemTrackers[n].genId = -1;
        }
    }
}

spawner::WAVEDATA* spawner::getUnusedWaveData()
{
    for (int i=0; i<NUM_WAVEDATA; i++)
    {
        if (mWaveData[i].mWaveType == WAVETYPE_UNUSED)
        {
            return &mWaveData[i];
        }
    }
    return NULL;
}

int spawner::numWaveData()
{
    int num = 0;

    for (int i=0; i<NUM_WAVEDATA; i++)
    {
        if (mWaveData[i].mWaveType != WAVETYPE_UNUSED)
        {
            ++num;
        }
    }

    return num;
}

void spawner::addEntityToWaveTracker(WAVEDATA* wd, entity* e)
{
    for (int i=0; i<NUM_WAVEITEMTRACKERS; i++)
    {
        if (wd->mItemTrackers[i].e == NULL)
        {
            wd->mItemTrackers[i].e = e;
            wd->mItemTrackers[i].genId = e->getGenId();
            break;
        }
    }
}
