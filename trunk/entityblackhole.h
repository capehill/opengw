#ifndef ENTITYBLACKHOLE_H
#define ENTITYBLACKHOLE_H

#include "entity.h"

class entityBlackHole : public entity
{
public:
    entityBlackHole();

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
    
    BOOL mActivated;
    float mAnimationIndex;
    float mAnimationSpeed;
    float mAnimationDepth;

    float mAttractorStrengthIndex;

    float mBalance;
    float mBalanceRate;

    float mStrength;

    int mPoints;

private:
    void drawRing();

    float mHumSpeed;
    float mHumSpeedTarget;
    int mHumLoopSoundId;
    int mFeedCount;
};

#endif // ENTITYBLACKHOLE_H
