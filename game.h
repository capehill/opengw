#pragma once

#include "defines.h"
#include "sound.h"
#include "scene.h"
#include "point3d.h"
#include "grid.h"
#include "particle.h"
#include "attractor.h"
#include "controls.h"
#include "mathutils.h"
#include "entity.h"
#include "blackholes.h"
#include "font.h"
#include "spawner.h"
#include "bomb.h"
#include "highscore.h"
#include "settings.h"

#include <memory>

class camera;
class stars;
class players;
class enemies;

enum
{
    SOUNDID_MUSICLOOP=0,

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
        GAMEMODE_ATTRACT=0,
        GAMEMODE_CREDITED,
        GAMEMODE_CHOOSE_GAMETYPE,
        GAMEMODE_PLAYING,
		GAMEMODE_HIGHSCORES_CHECK,
        GAMEMODE_HIGHSCORES,
        GAMEMODE_GAMEOVER_TRANSITION,
        GAMEMODE_GAMEOVER
    }GameMode;

    typedef enum
    {
        GAMETYPE_SINGLEPLAYER=0,
        GAMETYPE_MULTIPLAYER_COOP,
        GAMETYPE_MULTIPLAYER_VS
    }GameType;

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

    static settings mSettings;
    static sound mSound;
    std::unique_ptr<grid> mGrid;
    std::unique_ptr<enemies> mEnemies;
    std::unique_ptr<particle> mParticles;
    std::unique_ptr<camera> mCamera;
    static attractor mAttractors;
    static controls mControls;
    std::unique_ptr<stars> mStars;
    std::unique_ptr<players> mPlayers;
    static blackholes mBlackHoles;
    static spawner mSpawner;
    static bomb mBomb;
	static highscore mHighscore;

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
    }PointDisplay;

    static PointDisplay* mPointDisplays;

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
