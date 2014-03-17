#ifndef ENTITYPLAYER2_H
#define ENTITYPLAYER2_H

#include "player.h"

class entityPlayer2 : public player
{
public:
    entityPlayer2();
    ~entityPlayer2();

    virtual void initPlayerForGame();
    virtual void spawnTransition();

};

#endif // ENTITYPLAYER2_H
