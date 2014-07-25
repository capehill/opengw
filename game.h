#ifndef GAME_H
#define GAME_H


#include "defines.h"
#include "sound.h"
#include "scene.h"
#include "point3d.h"
#include "grid.h"
#include "particle.h"
#include "camera.h"
#include "attractor.h"
#include "controls.h"
#include "mathutils.h"
#include "entity.h"
#include "enemies.h"
#include "stars.h"
#include "players.h"
#include "blackholes.h"
#include "font.h"
#include "spawner.h"
#include "bomb.h"
#include "highscore.h"
#include "settings.h"


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

    void run();
    void draw(int pass);

    void startGame(GameType gameType);
    void endGame();

    static void showMessageAtLocation(char* message, const Point3d& pos, const vector::pen& pen);

    void startBomb();

    int numPlayers();

    static sound mSound;
    static grid mGrid;
    static particle mParticles;
    static camera mCamera;
    static attractor mAttractors;
    static controls mControls;
    static enemies mEnemies;
    static stars mStars;
    static players mPlayers;
    static blackholes mBlackHoles;
    static spawner mSpawner;
    static bomb mBomb;
	static highscore mHighscore;
    static settings mSettings;

    static int mSkillLevel;

    static GameMode mGameMode;

    static GameType mGameType;

    static BOOL mPaused;

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
        BOOL enabled;
    }PointDisplay;

    static PointDisplay* mPointDisplays;

    void runPointDisplays();
    void drawPointDisplays();
    void clearPointDisplays();

    float mMusicSpeed;
    float mMusicSpeedTarget;

    int mGameOverTimer;
    int mWeaponChangeTimer;

    float mBrightness;

    bool mDebounce;

    entity* mAttractModeBlackHoles[4];

};

extern game theGame;

#endif // GAME_H
