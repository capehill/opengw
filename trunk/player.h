#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "entityplayermissile.h"

class player : public entity
{
public:
    player();
    ~player();

    virtual void initPlayerForGame();

    virtual void run();
    virtual void spawn();
    virtual void spawnTransition();
    virtual void destroyTransition();
    virtual void indicating();

    virtual void draw();

    entityPlayerMissile* missiles;
    static const int mMaxMissiles;

    int mScore;
    int mMultiplier;

    int mPlayerAssignment;

    void addKillAtLocation(int points, Point3d pos);

    void runMissiles();

    bool shields() { return mSheildTimer > 0; }

    int getNumLives();
    int getNumBombs();
    int getGeoms();

    void addLife();
    void takeLife();
    void addBomb();
    void takeBomb();
    void addGeom(int value);
    void clearGeoms();
    void switchWeapons();

    void addPointsNoMultiplier(int points);

protected:

    void firePattern1(const Point3d& fireAngle, const Point3d& playerSpeed);
    void firePattern2(const Point3d& fireAngle, const Point3d& playerSpeed);
    void firePattern3(const Point3d& fireAngle, const Point3d& playerSpeed);

    int mCurrentWeapon;

    int mWeaponCounter;
    int mBombCounter;
    int mLifeCounter;

    BOOL mDrawSheild;

    int mFiringTimer;
    int mSheildTimer;
    int mBombInterimTimer;

    int mKillCounter;
    int mLevelAdvanceCounter;

    int mNumLives;
    int mNumBombs;
    int mGeoms;
};

#define PLAYER_SHEILD_TIME 250

#endif // PLAYER_H
