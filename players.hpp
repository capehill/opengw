#pragma once

#include "player.hpp"

#include <memory>

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

    player* getPlayer1() { return mPlayer1.get(); }
    player* getPlayer2() { return mPlayer2.get(); }
    player* getPlayer3() { return mPlayer3.get(); }
    player* getPlayer4() { return mPlayer4.get(); }

    static std::unique_ptr<player> mPlayer1;
    static std::unique_ptr<player> mPlayer2;
    static std::unique_ptr<player> mPlayer3;
    static std::unique_ptr<player> mPlayer4;

    const game& mGame;
};
