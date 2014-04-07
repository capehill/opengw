#ifndef ENTITYPLAYER4_H
#define ENTITYPLAYER4_H

#include "player.h"

class entityPlayer4 : public player
{
public:
    entityPlayer4();
    ~entityPlayer4();

    virtual void initPlayerForGame();
    virtual void spawnTransition();

};

#endif // ENTITYPLAYER4_H
