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

//    void clearWaveListEntities();

private:
    void transition();

    void spawnEntities(entity::EntityType type, int numWanted);
    void runWaves();

//    void addToWaveList(entity* e);
//    int numWaveListEntities();
//    void runWaveListEntities();

    void clearWaveData();
    void newWave(WAVETYPE mWaveType, entity::EntityType entityType, int spawnCount);
    WAVEDATA* getUnusedWaveData();
    int numWaveData();
    void addEntityToWaveTracker(WAVEDATA* wd, entity* e);

    float mSpawnIndex;
    int mLastSpawnIndex;
    float mSpawnProgress;

    int mSpawnCheckTimer;

    int mSpawnWaitTimer;

//    int mWaveIntervalTimer;
//    int mWaveStartTimer;
//    int mWaveEntityCounter;
//    WAVETYPE mWaveType;
//    entity::EntityType mWaveEntity;

/*
    typedef struct
    {
        entity* e;
        int genId;
    } WaveListItem;

    WaveListItem mWaveList[NUM_WAVELIST];
*/
    int mWaveStartTimer;
    WAVEDATA mWaveData[NUM_WAVEDATA];
};

#endif
