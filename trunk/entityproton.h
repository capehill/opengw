#ifndef ENTITY_PROTON_H
#define ENTITY_PROTON_H

#include "entity.h"

class entityProton : public entity
{
public:
    entityProton(void);

    virtual void run();
    virtual void spawnTransition();

private:
    void drawRing();

};

#endif

