#pragma once

#include "point3d.hpp"
#include "sound.hpp"
#include "vector.hpp"

#include <memory>
#include <vector>

// Forward declare pointers
class attractor;
class blackholes;
class bomb;
class camera;
class controls;
class enemies;
class entity;
class grid;
class highscore;
class particle;
class player;
class players;
class spawner;
class stars;

enum
{
    SOUNDID_MUSICLOOP = 0,

    SOUNDID_MENU_MUSICLOOP,

    SOUNDID_MENU_SELECT,

    SOUNDID_BACKGROUND_NOISELOOP,

    SOUNDID_PLAYERSPAWN,
    SOUNDID_PLAYERHIT,
    SOUNDID_PLAYERDEAD,
    SOUNDID_SHIELDSLOST,
    SOUNDID_PLAYERTHRUST,

    SOUNDID_EXTRALIFE,
    SOUNDID_EXTRABOMB,

    SOUNDID_BOMB,

    SOUNDID_MULTIPLIERADVANCE,

    SOUNDID_MISSILEHITWALL,

    SOUNDID_REPULSORA,
    SOUNDID_REPULSORB,
    SOUNDID_REPULSORC,
    SOUNDID_REPULSORD,

    SOUNDID_GRAVITYWELLDESTROYED,
    SOUNDID_GRAVITYWELLABSORBED,
    SOUNDID_GRAVITYWELLHIT,
    SOUNDID_GRAVITYWELLALERT,
    SOUNDID_GRAVITYWELLEXPLODE,

    SOUNDID_GRAVITYWELL_HUMLOOPA,
    SOUNDID_GRAVITYWELL_HUMLOOPB,
    SOUNDID_GRAVITYWELL_HUMLOOPC,
    SOUNDID_GRAVITYWELL_HUMLOOPD,
    SOUNDID_GRAVITYWELL_HUMLOOPE,
    SOUNDID_GRAVITYWELL_HUMLOOPF,

    SOUNDID_ENEMYSPAWN1,
    SOUNDID_ENEMYSPAWN2,
    SOUNDID_ENEMYSPAWN3,
    SOUNDID_ENEMYSPAWN4,
    SOUNDID_ENEMYSPAWN5,
    SOUNDID_ENEMYSPAWN6,

    SOUNDID_ENEMYHIT,

    SOUNDID_MAYFLIES,

    SOUNDID_PLAYERFIRE1,
    SOUNDID_PLAYERFIRE2,
    SOUNDID_PLAYERFIRE3,

    SOUNDID_LAST
};

class game
{
  public:
    typedef enum
    {
        GAMEMODE_ATTRACT = 0,
        GAMEMODE_CREDITED,
        GAMEMODE_CHOOSE_GAMETYPE,
        GAMEMODE_PLAYING,
        GAMEMODE_HIGHSCORES_CHECK,
        GAMEMODE_HIGHSCORES,
        GAMEMODE_GAMEOVER_TRANSITION,
        GAMEMODE_GAMEOVER
    } GameMode;

    typedef enum
    {
        GAMETYPE_SINGLEPLAYER = 0,
        GAMETYPE_MULTIPLAYER_COOP,
        GAMETYPE_MULTIPLAYER_VS
    } GameType;

    game();
    ~game();
    void quitThreads();

    void run();
    void draw(int pass);

    void startGame(GameType gameType);
    void endGame();

    static void showMessageAtLocation(char* message, const Point3d& pos, const vector::pen& pen);

    void startBomb();

    int numPlayers() const;

    player* getPlayer1() const;
    player* getPlayer2() const;
    player* getPlayer3() const;
    player* getPlayer4() const;

    std::unique_ptr<sound> mSound;
    std::unique_ptr<grid> mGrid;
    std::unique_ptr<enemies> mEnemies;
    std::unique_ptr<particle> mParticles;
    std::unique_ptr<camera> mCamera;
    std::unique_ptr<attractor> mAttractors;
    std::unique_ptr<controls> mControls;
    std::unique_ptr<stars> mStars;
    std::unique_ptr<players> mPlayers;
    std::unique_ptr<blackholes> mBlackHoles;
    std::unique_ptr<spawner> mSpawner;
    std::unique_ptr<bomb> mBomb;
    std::unique_ptr<highscore> mHighscore;

    static int mSkillLevel;

    static GameMode mGameMode;

    static GameType mGameType;

    static bool mPaused;

    static int mCredits;
    static int mLevel;

    static int m2PlayerNumLives;
    static int m2PlayerNumBombs;

  private:
    typedef struct
    {
        Point3d pos;
        vector::pen pen;
        char message[128];
        int timer;
        bool enabled;
    } PointDisplay;

    static std::vector<PointDisplay> mPointDisplays;

    void runPointDisplays();
    void drawPointDisplays();
    void clearPointDisplays();

    float mMusicSpeed { 0.0f };
    float mMusicSpeedTarget { 0.0f };

    int mGameOverTimer { 0 };
    int mWeaponChangeTimer { 0 };

    float mBrightness;

    bool mDebounce { false };

    std::unique_ptr<entity> mAttractModeBlackHoles[4];
};

extern std::unique_ptr<game> theGame;
