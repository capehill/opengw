#ifndef PLAYERS_H
#define PLAYERS_H

#include "player.h"
#include "entityPlayer1.h"
#include "entityPlayer2.h"
#include "entityPlayer3.h"
#include "entityPlayer4.h"

class players
{
public:
    players();
    ~players();

    void run();
    void draw();

    player* getPlayerClosestToPosition(const Point3d& point);

    static player* mPlayer1;
    static player* mPlayer2;
    static player* mPlayer3;
    static player* mPlayer4;
};

#endif // PLAYERS_H
