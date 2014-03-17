#ifndef ENTITYPLAYER1_H
#define ENTITYPLAYER1_H

#include "player.h"

class entityPlayer1 : public player
{
public:
    entityPlayer1();
    ~entityPlayer1();

    virtual void initPlayerForGame();
    virtual void spawnTransition();

};

#endif // ENTITYPLAYER1_H
