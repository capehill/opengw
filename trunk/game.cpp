#include "game.h"

// Statics
sound game::mSound;
grid game::mGrid;
particle game::mParticles;
camera game::mCamera;
attractor game::mAttractors;
controls game::mControls;
enemies game::mEnemies;
stars game::mStars;
players game::mPlayers;
blackholes game::mBlackHoles;
spawner game::mSpawner;
bomb game::mBomb;
highscore game::mHighscore;


#define NUM_POINT_DISPLAYS 40
#define TIME_POINT_DISPLAY 50

extern scene oglScene;

int game::mSkillLevel;
game::GameMode game::mGameMode;
game::PointDisplay* game::mPointDisplays;
BOOL game::mFreeplay = TRUE;
int game::mCredits = 0;
int game::mNumPlayers = 1;
int game::mLevel = 0;
int game::m2PlayerNumLives = 0;
int game::m2PlayerNumBombs = 0;


game::game()
{
    //
    // Load our sounds
    //

    mSound.loadTrack("sounds/musicloop.wav", SOUNDID_MUSICLOOP, .2, true);
    mSound.loadTrack("sounds/menumusicloop.wav", SOUNDID_MENU_MUSICLOOP, .2, true);

    mSound.loadTrack("sounds/backgroundnoiseloop.wav", SOUNDID_BACKGROUND_NOISELOOP, .2, true);

    mSound.loadTrack("sounds/playerspawn.wav", SOUNDID_PLAYERSPAWN, 1, false);
    mSound.loadTrack("sounds/playerhit.wav", SOUNDID_PLAYERHIT, .5, false);
    mSound.loadTrack("sounds/playerdead.wav", SOUNDID_PLAYERDEAD, .5, false);
    mSound.loadTrack("sounds/sheildsdown.wav", SOUNDID_SHIELDSLOST, .7, false);
    mSound.loadTrack("sounds/playerthrust.wav", SOUNDID_PLAYERTHRUST, .2, true);

    mSound.loadTrack("sounds/extralife.wav", SOUNDID_EXTRALIFE, .3, false);
    mSound.loadTrack("sounds/extrabomb.wav", SOUNDID_EXTRABOMB, .3, false);

    mSound.loadTrack("sounds/bomb.wav", SOUNDID_BOMB, .5, false);

    mSound.loadTrack("sounds/multiplieradvance.wav", SOUNDID_MULTIPLIERADVANCE, .3, false);

    mSound.loadTrack("sounds/missilehitwall.wav", SOUNDID_MISSILEHITWALL, .3, false);

    mSound.loadTrack("sounds/gravitywelldestroyed.wav", SOUNDID_GRAVITYWELLDESTROYED, .3, false);
    mSound.loadTrack("sounds/gravitywelldestroyed.wav", SOUNDID_GRAVITYWELLABSORBED, .1, false);
    mSound.loadTrack("sounds/gravitywellhit.wav", SOUNDID_GRAVITYWELLHIT, .3, false);
    mSound.loadTrack("sounds/gravitywellalert.wav", SOUNDID_GRAVITYWELLALERT, .5, false);
    mSound.loadTrack("sounds/gravitywellexplode.wav", SOUNDID_GRAVITYWELLEXPLODE, .5, false);

    mSound.loadTrack("sounds/gravitywellhumloop.wav", SOUNDID_GRAVITYWELL_HUMLOOPA, .3, true);
    mSound.loadTrack("sounds/gravitywellhumloop.wav", SOUNDID_GRAVITYWELL_HUMLOOPB, .3, true);
    mSound.loadTrack("sounds/gravitywellhumloop.wav", SOUNDID_GRAVITYWELL_HUMLOOPC, .3, true);
    mSound.loadTrack("sounds/gravitywellhumloop.wav", SOUNDID_GRAVITYWELL_HUMLOOPD, .3, true);
    mSound.loadTrack("sounds/gravitywellhumloop.wav", SOUNDID_GRAVITYWELL_HUMLOOPE, .3, true);
    mSound.loadTrack("sounds/gravitywellhumloop.wav", SOUNDID_GRAVITYWELL_HUMLOOPF, .3, true);

    mSound.loadTrack("sounds/enemyspawn1.wav", SOUNDID_ENEMYSPAWN1, .5, false);
    mSound.loadTrack("sounds/enemyspawn2.wav", SOUNDID_ENEMYSPAWN2, .5, false);
    mSound.loadTrack("sounds/enemyspawn3.wav", SOUNDID_ENEMYSPAWN3, .5, false);
    mSound.loadTrack("sounds/enemyspawn4.wav", SOUNDID_ENEMYSPAWN4, .5, false);
    mSound.loadTrack("sounds/enemyspawn5.wav", SOUNDID_ENEMYSPAWN5, .3, false);
    mSound.loadTrack("sounds/enemyspawn6.wav", SOUNDID_ENEMYSPAWN6, .3, false);

    mSound.loadTrack("sounds/enemyhit.wav", SOUNDID_ENEMYHIT, .5, false);

    mSound.loadTrack("sounds/mayflies.wav", SOUNDID_MAYFLIES, .1, true);

    mSound.loadTrack("sounds/playerfire1.wav", SOUNDID_PLAYERFIRE1, .9, true);
    mSound.loadTrack("sounds/playerfire2.wav", SOUNDID_PLAYERFIRE2, .5, true);
    mSound.loadTrack("sounds/playerfire3.wav", SOUNDID_PLAYERFIRE3, .8, true);

    mSound.startSound();

    mBrightness = 0;

    mCamera.center();

    mPointDisplays = new PointDisplay[NUM_POINT_DISPLAYS];
    for (int i=0; i<NUM_POINT_DISPLAYS; i++)
    {
        mPointDisplays[i].enabled = FALSE;
    }

    // Tag 4 black holes for attract mode
    for (int i=0; i<4; i++)
    {
        mAttractModeBlackHoles[i] = new entity();
        mAttractModeBlackHoles[i]->setPos(Point3d(mathutils::frandFrom0To1() *theGame.mGrid.extentX(), mathutils::frandFrom0To1() * theGame.mGrid.extentY(), 0));
        mAttractModeBlackHoles[i]->setEdgeBounce(FALSE);

        static float heading=mathutils::frandFrom0To1() * (2*PI);
        if ((mathutils::frandFrom0To1() * 40) < 1)
        {
            // Pick a random direction
            const float variation = 1.5;
            heading + mathutils::frandFrom0To1() * (2*PI);
        }

        Point3d speedVector = Point3d(1,0,0);
        speedVector = mathutils::rotate2dPoint(speedVector, heading);
        mAttractModeBlackHoles[i]->setDrift(speedVector);
    }

    mGameMode = GAMEMODE_ATTRACT;

    mSound.playTrack(SOUNDID_MENU_MUSICLOOP);
}

game::~game()
{
    delete mPointDisplays;
}

void game::run()
{
    static bool creditButtonLast = FALSE;
    bool creditButton = FALSE; // mControls.getCreditButton();
    if (creditButton && !creditButtonLast)
    {
        ++mCredits;
    }
    creditButtonLast = creditButton;

    if (mFreeplay)
    {
        mCredits = 4;
    }

    mAttractors.clearAll();

    // Run the camera
    mCamera.run();

    // Run the point displays
    runPointDisplays();

    switch(mGameMode)
    {
        case GAMEMODE_ATTRACT:
            {
                if (mCredits > 0)
                {
                    mGameMode = GAMEMODE_CREDITED;
                }
            }
            break;
        case GAMEMODE_CREDITED:
            {
                // IT'S STUPID TO HAVE A START BUTTON FOR EACH NUMBER OF PLAYERS
                // I'D RATHER GO WITH A "JOIN" STAGE OF STARTING THE GAME
                if (mControls.getStart1Button())
                {
                    startGame(1);
                }
                else if (mControls.getStart2Button())
                {
                    startGame(2);
                }
                else if (mControls.getStart3Button())
                {
                    startGame(3);
                }
                else if (mControls.getStart4Button())
                {
                    startGame(4);
                }
            }
            break;
        case GAMEMODE_PLAYING:
            {
                mCamera.followPlayer();
                mStars.run();
                mBlackHoles.run();
                mEnemies.run();
                mPlayers.run();
                mBomb.run();
                mSpawner.run();

                // Music speed

                mMusicSpeedTarget = 1;

                // Slow the music down when someone is respawning
                if (this->mNumPlayers >= 1)
                {
                    if (game::mPlayers.mPlayer1->getState() == entity::ENTITY_STATE_DESTROYED)
                        mMusicSpeedTarget = .5;
                }
                if (this->mNumPlayers >= 2)
                {
                    if (game::mPlayers.mPlayer2->getState() == entity::ENTITY_STATE_DESTROYED)
                        mMusicSpeedTarget = .5;
                }
                if (this->mNumPlayers >= 3)
                {
                    if (game::mPlayers.mPlayer3->getState() == entity::ENTITY_STATE_DESTROYED)
                        mMusicSpeedTarget = .5;
                }
                if (this->mNumPlayers >= 4)
                {
                    if (game::mPlayers.mPlayer4->getState() == entity::ENTITY_STATE_DESTROYED)
                        mMusicSpeedTarget = .5;
                }

                if (mMusicSpeed < mMusicSpeedTarget)
				{
                    mMusicSpeed += .01;
					if (mMusicSpeed > mMusicSpeedTarget)
						mMusicSpeed = mMusicSpeedTarget;
				}
                else if (mMusicSpeed > mMusicSpeedTarget)
				{
                    mMusicSpeed -= .01;
					if (mMusicSpeed < mMusicSpeedTarget)
						mMusicSpeed = mMusicSpeedTarget;
				}

                mSound.setTrackSpeed(SOUNDID_MUSICLOOP, mMusicSpeed);

                // Sound cutoff

                if (mSoundCutoff < mSoundCutoffTarget)
				{
                    mSoundCutoff += .005;
					if (mSoundCutoff > mMusicSpeedTarget)
						mSoundCutoff = mMusicSpeedTarget;
				}

                mSound.setCutoffFreq(mSoundCutoff);

            }
            break;
        case GAMEMODE_HIGHSCORES:
			mHighscore.run();
			break;
        case GAMEMODE_HIGHSCORES_CHECK:
			if (mHighscore.isHighScore(mPlayers.mPlayer1->mScore) == true)
			{
				mGameMode = GAMEMODE_HIGHSCORES;
				mHighscore.init();
				break;
			}
            oglScene.showHighScores();
            break;
        case GAMEMODE_GAMEOVER_TRANSITION:
			mGameMode = GAMEMODE_GAMEOVER;
            mGameOverTimer = 0;
            break;
        case GAMEMODE_GAMEOVER:

            if (mBrightness > 0)
            {
                mBrightness *= .98;
            }

            mSound.setCutoffFreq(1);

            ++mGameOverTimer;
            if (mGameOverTimer > 180)
            {
    			if (game::mNumPlayers == 1)
					game::mGameMode = game::GAMEMODE_HIGHSCORES_CHECK;
                else // TODO - MULTIPLAYER HIGH SCORES?????
                    mGameMode = GAMEMODE_ATTRACT;
            }
            break;
    }

	if (mGameMode == GAMEMODE_ATTRACT || mGameMode == GAMEMODE_CREDITED)
	{
        if (mBrightness > 0)
        {
            mBrightness *= .98;
        }
	}

    mGrid.run();

	if ((game::mGameMode == game::GAMEMODE_HIGHSCORES_CHECK) || (game::mGameMode == game::GAMEMODE_HIGHSCORES))
	{
	}
    else if (mGameMode == GAMEMODE_ATTRACT || mGameMode == GAMEMODE_CREDITED)
    {
        mCamera.center();

        // Attractors to wander around the fireworks display

        int sizex = mGrid.extentX();
        int sizey = mGrid.extentY();

        for (int i=0; i<4; i++)
        {
            mAttractModeBlackHoles[i]->setState(entity::ENTITY_STATE_RUNNING);
            mAttractModeBlackHoles[i]->run();
            
            Point3d pos = mAttractModeBlackHoles[i]->getPos();

            {
                attractor::Attractor* att = game::mAttractors.getAttractor();
                if (att)
                {
                    att->strength = -40;
                    att->zStrength = 0;
                    att->radius = 40;
                    att->pos = pos;
                    att->enabled = TRUE;
                    att->attractsParticles = TRUE;
                }
            }

            for (int j=0; j<4; j++)
            {
                if (i == j) continue;

                float angle = mathutils::calculate2dAngle(mAttractModeBlackHoles[j]->getPos(), mAttractModeBlackHoles[i]->getPos());
                float distance = mathutils::calculate2dDistance(mAttractModeBlackHoles[j]->getPos(), mAttractModeBlackHoles[i]->getPos());

                float strength = 2;
                if (distance < mAttractModeBlackHoles[i]->getRadius())
                {
                    distance = mAttractModeBlackHoles[i]->getRadius();
                }

                float r = 1.0/(distance*distance);

                // Add a slight curving vector to the gravity
                Point3d gravityVector(r * strength, 0, 0);
                Point3d g = mathutils::rotate2dPoint(gravityVector, angle+.4);

                Point3d speed = mAttractModeBlackHoles[j]->getDrift();
                speed.x += g.x;
                speed.y += g.y;
                mAttractModeBlackHoles[j]->setDrift(speed);
            }

            float heading;
            if ((mathutils::frandFrom0To1() * 40) < 1)
            {
                // Pick a random direction
                const float variation = 1.5;
                heading + mathutils::frandFrom0To1() * (2*PI);
            }

            // Change direction when we hit the grid edges

            Point3d mSpeed = mAttractModeBlackHoles[i]->getDrift();
            Point3d mPos = mAttractModeBlackHoles[i]->getPos();

            const float leftEdge = 2;
            const float bottomEdge = 2;
            const float rightEdge = (sizex - 2)-1;
            const float topEdge = (sizey - 2)-1;

            BOOL hitEdge = FALSE;
            if (mPos.x <= leftEdge)
            {
                mSpeed.x = -mSpeed.x;
                mPos.x = leftEdge;
            }
            else if (mPos.x >= rightEdge)
            {
                mSpeed.x = -mSpeed.x;
                mPos.x = rightEdge;
            }
            if (mPos.y <= bottomEdge)
            {
                mSpeed.y = -mSpeed.y;
                mPos.y = bottomEdge;
            }
            else if (mPos.y >= topEdge)
            {
                mSpeed.y = -mSpeed.y;
                mPos.y = topEdge;
            }

            mAttractModeBlackHoles[i]->setDrift(mSpeed);
            mAttractModeBlackHoles[i]->setPos(mPos);

        }

        // Fireworks display
        static int fw = 99999;
        ++fw;
        if (fw >= 5)
        {
            fw= 0;
            static float colorTimer=0;
            colorTimer += .08;

            Point3d pos(mathutils::frandFrom0To1() * sizex, mathutils::frandFrom0To1() * sizey);

            {
                Point3d angle(0, 0, 0);
                float speed = mathutils::frandFrom0To1() * 4;
                float spread = (2*PI);
                int num = 100;
                int timeToLive = 99999;
                vector::pen pen;

                pen.r = get_sin(colorTimer+((2*PI)/1));
                pen.g = get_sin(colorTimer+((2*PI)/2));
                pen.b = get_sin(colorTimer+((2*PI)/3));

                if (pen.r < 0) pen.r = 0;
                if (pen.g < 0) pen.g = 0;
                if (pen.b < 0) pen.b = 0;

                pen.r += .4;
                pen.g += .4;
                pen.b += .4;

                pen.a = 100;
                pen.lineRadius=4;
                mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, TRUE, FALSE, .98, TRUE);
            }
        }
    }

    mParticles.run();
}

#define GRID_GLOW // PERFORMANCE: Making the grid glow causes us to have to draw it twice, which is slower
#define PARTICLE_GLOW // PERFORMANCE: Uncomment to get glowing particles, but it has a performance hit!

void game::draw(int pass)
{
    mGrid.brightness = mBrightness;

    // The camera
    {
        glTranslatef(-mCamera.mCurrentPos.x, -mCamera.mCurrentPos.y, -mCamera.mCurrentPos.z);
    }

    {
        // Grid
        if (((mGameMode == GAMEMODE_PLAYING) || (mGameMode == GAMEMODE_GAMEOVER) || (mGameMode == GAMEMODE_GAMEOVER_TRANSITION))
#ifndef GRID_GLOW
            && (pass == scene::RENDERPASS_PRIMARY)
#endif
            )
        {
            glLineWidth(5);
            mGrid.draw();
        }

        // Particles
        if (pass == scene::RENDERPASS_PRIMARY)
        {
            //glEnable(GL_LINE_SMOOTH);
            //glEnable(GL_MULTISAMPLE);
            glLineWidth(4);

            mParticles.draw();

            //glDisable(GL_MULTISAMPLE);
            //glDisable(GL_LINE_SMOOTH);
        }
        else
        {
#ifdef PARTICLE_GLOW
            glLineWidth(10);
            mParticles.draw();
#endif
        }

        // Enemies
        if (mGameMode == GAMEMODE_PLAYING)
        {
            glLineWidth(4);

            if (pass == scene::RENDERPASS_PRIMARY)
            {
//                glEnable(GL_LINE_SMOOTH);
//                glEnable(GL_MULTISAMPLE);

                mEnemies.draw();

//                glDisable(GL_MULTISAMPLE);
//                glDisable(GL_LINE_SMOOTH);
            }
            else
            {
                mEnemies.draw();
            }
        }

        // Players
        if (mGameMode == GAMEMODE_PLAYING)
        {
            glLineWidth(4);
            glPointSize(4/2);

            if (pass == scene::RENDERPASS_PRIMARY)
            {
//                glEnable(GL_LINE_SMOOTH);
//                glEnable(GL_MULTISAMPLE);

                mPlayers.draw();

//                glDisable(GL_MULTISAMPLE);
//                glDisable(GL_LINE_SMOOTH);
            }
            else
            {
                mPlayers.draw();
            }
        }

        // Stars
        if (pass == scene::RENDERPASS_PRIMARY)
        {
//            glEnable(GL_POINT_SMOOTH);
//            glEnable(GL_MULTISAMPLE);

            mStars.draw();

//            glDisable(GL_MULTISAMPLE);
//            glDisable(GL_POINT_SMOOTH);
        }

        // Bombs
        {
            glLineWidth(4);
            mBomb.draw();
        }

        // Point displays
        {
            glLineWidth(4);

            if (pass == scene::RENDERPASS_PRIMARY)
            {
//                glEnable(GL_LINE_SMOOTH);
//                glEnable(GL_MULTISAMPLE);

                drawPointDisplays();

//                glDisable(GL_MULTISAMPLE);
//                glDisable(GL_LINE_SMOOTH);
            }
            else
            {
                drawPointDisplays();
            }
        }

	}
}

void game::startGame(int numPlayers)
{
    mNumPlayers = numPlayers;

    mBrightness = 1;

    mCamera.center();
    mCamera.mCurrentZoom = 120;

    mLevel = 0;

    mSkillLevel = 0;

    mSpawner.init();

    // Fire up the players

    if (mNumPlayers >= 1)
    {
        this->mPlayers.mPlayer1->initPlayerForGame();
    }
    if (mNumPlayers >= 2)
    {
        this->mPlayers.mPlayer2->initPlayerForGame();
    }
    if (mNumPlayers >= 3)
    {
        this->mPlayers.mPlayer3->initPlayerForGame();
    }
    if (mNumPlayers >= 4)
    {
        this->mPlayers.mPlayer4->initPlayerForGame();
    }

    if (mNumPlayers > 1)
    {
        // Shared lives and bombs
        m2PlayerNumLives = 5;
        m2PlayerNumBombs = 0;
    }

    mGameMode = GAMEMODE_PLAYING;

    mMusicSpeed = 1;
    mMusicSpeedTarget = 1;

    mWeaponChangeTimer = 0;

    mMusicSpeedTarget = 1;
    mMusicSpeed = 1;

    mSoundCutoffTarget = 1;
    mSoundCutoff = 1;

    mSound.stopTrack(SOUNDID_MENU_MUSICLOOP);
    mSound.playTrack(SOUNDID_MUSICLOOP);
    mSound.setTrackSpeed(SOUNDID_MUSICLOOP, mMusicSpeed);

    mSound.playTrack(SOUNDID_BACKGROUND_NOISELOOP);

    mParticles.killAll();
}

void game::endGame()
{
    // Doesn't actually end the game, just does some work that happens after the last player life is used

    mSound.stopAllTracks();

    mSound.playTrack(SOUNDID_PLAYERDEAD);
    mSound.playTrack(SOUNDID_MENU_MUSICLOOP);

    // Kill all players
    mPlayers.mPlayer1->setState(entity::ENTITY_STATE_INACTIVE);
    mPlayers.mPlayer2->setState(entity::ENTITY_STATE_INACTIVE);

    // Kill all enemies
    mEnemies.disableAllEnemies();

    // Kill all attractors
    mAttractors.clearAll();
}

void game::showMessageAtLocation(char* message, const Point3d& pos, const vector::pen& pen)
{
    // Find an empty diplay
    for (int i=0; i<NUM_POINT_DISPLAYS; i++)
    {
        if (!mPointDisplays[i].enabled)
        {
            mPointDisplays[i].enabled = TRUE;
            mPointDisplays[i].pos = pos;
            mPointDisplays[i].pen = pen;
            sprintf(mPointDisplays[i].message, message);
            mPointDisplays[i].timer = TIME_POINT_DISPLAY;
            break;
        }
    }
}

void game::startBomb()
{
    mSoundCutoff = .001;
    mSoundCutoffTarget = 1;
}

void game::runPointDisplays()
{
    for (int i=0; i<NUM_POINT_DISPLAYS; i++)
    {
        if (mPointDisplays[i].enabled)
        {
            mPointDisplays[i].timer--;
            if (mPointDisplays[i].timer <= 0)
            {
                mPointDisplays[i].enabled = FALSE;
            }
        }
    }
}

void game::drawPointDisplays()
{
    for (int i=0; i<NUM_POINT_DISPLAYS; i++)
    {
        if (mPointDisplays[i].enabled)
        {
            float percent = (float)mPointDisplays[i].timer / TIME_POINT_DISPLAY;
            percent *= 2;
            if (percent > 1) percent = 1;

            float scale = .9 * percent;
            float alpha = 1 * percent;

            vector::pen pen = mPointDisplays[i].pen;
            pen.a=alpha;
            pen.lineRadius=3;

            font::AlphanumericsPrint(font::ALIGN_CENTER, scale, mPointDisplays[i].pos.x, mPointDisplays[i].pos.y, &pen, mPointDisplays[i].message);
        }
    }
}

void game::clearPointDisplays()
{
    for (int i=0; i<NUM_POINT_DISPLAYS; i++)
    {
        mPointDisplays[i].enabled = FALSE;
    }
}


