#ifndef SPAWNER_H
#define SPAWNER_H

#include "entity.h"

const int NUM_WAVEITEMTRACKERS = 200; // Should be set to the maximum number of entities that can spawn during a way
const int NUM_WAVEDATA = 200; // The maximum number of simultanious waves

class spawner
{
public:

    typedef enum
    {
        WAVETYPE_UNUSED=0,
        WAVETYPE_SWARM,
        WAVETYPE_RUSH
    }WAVETYPE;

    typedef struct
    {
        entity* e;
        int genId;
    } WAVEITEMTRACKER;

    typedef struct
    {
        WAVETYPE mWaveType;
        entity::EntityType entityType;
        int spawnCount;
        int timer;
        WAVEITEMTRACKER mItemTrackers[NUM_WAVEITEMTRACKERS];
    }WAVEDATA;

    spawner(void);

    void run();
    void init();

    int getSpawnIndex() const;

private:
    void transition();

    void spawnEntities(entity::EntityType type, int numWanted);
    void runWaves();

    void clearWaveData();
    void newWave(WAVETYPE mWaveType, entity::EntityType entityType, int spawnCount);
    WAVEDATA* getUnusedWaveData();
    int numWaveData();
    void addEntityToWaveTracker(WAVEDATA* wd, entity* e);

    float mSpawnIndex;
    int mLastSpawnIndex;
    int mNumWavesAllowed;
    float mSpawnProgress;

    int mSpawnCheckTimer;

    int mSpawnWaitTimer;

    int mWaveStartTimer;
    WAVEDATA mWaveData[NUM_WAVEDATA];
};

#endif
