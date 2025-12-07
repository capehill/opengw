#pragma once

#include "player.hpp"

class game;

class players
{
  public:
    players(const game& gameRef);
    ~players();

    void run();
    void draw();

    player* getPlayerClosestToPosition(const Point3d& point);
    player* getRandomActivePlayer();

    static player* mPlayer1;
    static player* mPlayer2;
    static player* mPlayer3;
    static player* mPlayer4;

    const game& mGame;
};
