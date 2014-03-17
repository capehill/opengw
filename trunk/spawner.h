#ifndef SPAWNER_H
#define SPAWNER_H

#include "entity.h"

const int NUM_WAVELIST = 200;

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
        entity::EntityType entityType;
        int numWanted;
    }ENTITYITEM;

    spawner(void);

    void run();
    void init();

    int getSpawnIndex() const;

    void clearWaveListEntities();

private:
    void transition();

    void spawnEntities(entity::EntityType type, int numWanted);
    void runWaves();

    void addToWaveList(entity* e);
    int numWaveListEntities();
    void runWaveListEntities();

    float mSpawnIndex;
    int mLastSpawnIndex;

    int mSpawnCheckTimer;

    int mSpawnWaitTimer;

    int mWaveIntervalTimer;
    int mWaveStartTimer;
    int mWaveEntityCounter;
    WAVETYPE mWaveType;
    entity::EntityType mWaveEntity;

    typedef struct
    {
        entity* e;
        int genId;
    } WaveListItem;

    WaveListItem mWaveList[NUM_WAVELIST];
};

#endif
