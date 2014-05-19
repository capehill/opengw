#ifndef ENTITYREPULSOR_H
#define ENTITYREPULSOR_H

#include "entity.h"

class entityRepulsorShieldLine;

class entityRepulsor : public entity
{
public:
    entityRepulsor();

    virtual void run();
    virtual void spawnTransition();
    virtual void destroyTransition();
    virtual void indicateTransition();
    virtual void draw();

    void repelEntity(entity* e);

    typedef enum
    {
        State_Thinking = 0,
        State_Aiming,
        State_Charging
    }State;

    bool mShieldsEnabled;

protected:
    State mAIState;
    int mTimer;
    int mLoopSoundId;

    entityRepulsorShieldLine* mShield1;
    entityRepulsorShieldLine* mShield2;
    entityRepulsorShieldLine* mShield3;
};

#endif // ENTITYREPULSOR_H
