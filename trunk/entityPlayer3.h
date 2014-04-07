#ifndef ENTITYPLAYER3_H
#define ENTITYPLAYER3_H

#include "player.h"

class entityPlayer3 : public player
{
public:
    entityPlayer3();
    ~entityPlayer3();

    virtual void initPlayerForGame();
    virtual void spawnTransition();

};

#endif // ENTITYPLAYER3_H
