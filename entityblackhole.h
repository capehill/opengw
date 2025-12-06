#pragma once

#include "entity.h"

class game;

class entityBlackHole : public entity
{
public:
    entityBlackHole(const game& gameRef);

    virtual void runTransition();
    virtual void run();
    virtual void spawnTransition();
    virtual void destroyTransition();
    virtual void indicateTransition();

    virtual void hit(entity* aEntity);
    virtual entity* hitTest(const Point3d& pos, float radius);

    virtual void draw();

    void feed(int points);

    const float getRadius() const;

    bool mActivated;
    float mAnimationIndex;
    float mAnimationSpeed;
    float mAnimationDepth;

    float mAttractorStrengthIndex;

    float mBalance;
    float mBalanceRate;

    float mStrength;

    int mFeedCount { 0 };

    int mPoints { 0 };

private:
    void drawRing();

    float mHumSpeed { 0.0f };
    float mHumSpeedTarget { 0.0f };
    int mHumLoopSoundId;
    float mGridPullIndex;

    const game& mGame;
};
