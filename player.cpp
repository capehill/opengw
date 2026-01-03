#include "attractor.hpp"
#include "bomb.hpp"
#include "controls.hpp"
#include "defines.hpp"
#include "game.hpp"
#include "player.hpp"

#include <cstdio>

#include "SDL_opengl.h"

player::player()
    : entity()
{
    mDrawSheild = false;

    mScoreValue = 0;

    mSpawnTime = 40;

    mSheildTimer = PLAYER_SHEILD_TIME;

    mJoined = false;

    mDestroyTime = 40;

    mExhaustSpreadIndex = mathutils::randFromTo(0, 1);

    setState(ENTITY_STATE_INACTIVE);

    // Create our missiles
    missiles.resize(500);
    for (std::size_t i = 0; i < missiles.size(); i++) {
        entityPlayerMissile* missile = &missiles[i];
        missile->setEnabled(false);
    }

    initPlayerForGame();
}

player::~player()
{
}

void player::initPlayerForGame()
{
    mNumBombs = 5;
    mNumLives = 5;

    mWeaponCounter = 0;
    mBombCounter = 0;
    mLifeCounter = 0;

    mScore = 0;

    mMultiplier = 1;

    mFiringTimer = 0;

    mKillCounter = 0;
    mLevelAdvanceCounter = 0;

    mCurrentWeapon = 0;

    mAngle = 0;

    // Set all missiles to inactive
    for (std::size_t i = 0; i < missiles.size(); i++) {
        entityPlayerMissile* missile = &missiles[i];
        missile->setEnabled(false);
    }

    setState(entity::ENTITY_STATE_SPAWN_TRANSITION);

    mJoined = true;
}

void player::deinitPlayerForGame()
{
    mJoined = true;
}

void player::run()
{
    if (getEnabled()) {
        // Read the trigger
        if (theGame->mGameType == game::GAMETYPE_SINGLEPLAYER) {
            bool trigger = theGame->mControls->getTriggerButton(mPlayerAssignment);
            if (trigger) {
                if (getNumBombs() > 0 && mBombInterimTimer <= 0) {
                    // Fire off a bomb
                    takeBomb();
                    mBombInterimTimer = 50;

                    theGame->mBomb->startBomb(mPos, 1, 6, 2, 200, vector::pen(1, 1, 1, .3, 4));
                    theGame->mSound->playTrack(SOUNDID_BOMB);
                    theGame->startBomb();
                }
            }
            if (mBombInterimTimer > 0) {
                --mBombInterimTimer;
            }
        }

        Point3d playerSpeed(0, 0, 0);

        // Move the player
        Point3d leftStick = theGame->mControls->getLeftStick(mPlayerAssignment);
        float distance = mathutils::calculate2dDistance(Point3d(0, 0, 0), leftStick);
        if (distance > .1) {
            //
            // The movement stick is being used
            //

            if (distance > .6) {
                distance = 1;
            } else
                distance = .5;

            float angle = mathutils::calculate2dAngle(Point3d(0, 0, 0), leftStick) + mathutils::DegreesToRads(90);

            angle -= mathutils::DegreesToRads(90);

            // Rotate to the correct angle
            angle = mathutils::wrapRadians(angle);
            float currentAngle = mathutils::wrapRadians(this->getAngle());
            float diff = mathutils::diffAngles(angle, currentAngle);
            currentAngle += diff * .2;
            this->setAngle(currentAngle);

            // Move
            Point3d thrust(distance * 0.6, 0, 0);
            thrust = mathutils::rotate2dPoint(thrust, currentAngle + mathutils::DegreesToRads(90));
            playerSpeed = thrust;
            this->setPos(this->getPos() + thrust);

            // Emit exhaust particles
            Point3d exhaustAngle = currentAngle + mathutils::DegreesToRads(180);
            Point3d exhaustOffset;

            float speed = .8; // .5
            int num = 1;
            int timeToLive = 200;
            vector::pen pen = this->getExhaustPen();
            pen.a = 100;

            // Main stream
            {
                float spread = .1;
                exhaustOffset = mathutils::rotate2dPoint(Point3d(0, -2, 0), currentAngle);
                exhaustOffset += getPos();
                theGame->mParticles->emitter(&exhaustOffset, &exhaustAngle, speed, spread, num, &pen, timeToLive, true, true, .92, false);
            }
            // First swirl
            {
                exhaustOffset = mathutils::rotate2dPoint(Point3d(0, -3, 0), currentAngle + (get_sin(mExhaustSpreadIndex) * .3));
                exhaustOffset += getPos();

                float spread = 0;
                theGame->mParticles->emitter(&exhaustOffset, &exhaustAngle, speed, spread, num, &pen, timeToLive, true, true, .92, false);
            }
            // Second swirl
            {
                exhaustOffset = mathutils::rotate2dPoint(Point3d(0, -3, 0), currentAngle + (get_sin(-mExhaustSpreadIndex) * .3));
                exhaustOffset += getPos();

                float spread = 0;
                theGame->mParticles->emitter(&exhaustOffset, &exhaustAngle, speed, spread, num, &pen, timeToLive, true, true, .92, false);
            }
            mExhaustSpreadIndex += .18;

            if (!theGame->mSound->isTrackPlaying(SOUNDID_PLAYERTHRUST))
                theGame->mSound->playTrack(SOUNDID_PLAYERTHRUST);
        } else {
            theGame->mSound->stopTrack(SOUNDID_PLAYERTHRUST);
        }

        // Firing
        Point3d rightStick = theGame->mControls->getRightStick(mPlayerAssignment);
        distance = mathutils::calculate2dDistance(Point3d(0, 0, 0), rightStick);
        if (distance > .1) {
            //
            // The firing stick is being used
            //

            switch (mCurrentWeapon) {
            case 0:
                firePattern1(rightStick, playerSpeed);
                {
                    if (!theGame->mSound->isTrackPlaying(SOUNDID_PLAYERFIRE1))
                        theGame->mSound->playTrack(SOUNDID_PLAYERFIRE1);

                    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE2);
                    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE3);
                }
                break;
            case 1:
                firePattern2(rightStick, playerSpeed);
                {
                    if (!theGame->mSound->isTrackPlaying(SOUNDID_PLAYERFIRE2))
                        theGame->mSound->playTrack(SOUNDID_PLAYERFIRE2);

                    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE1);
                    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE3);
                }
                break;
            case 2:
                firePattern3(rightStick, playerSpeed);
                {
                    if (!theGame->mSound->isTrackPlaying(SOUNDID_PLAYERFIRE3))
                        theGame->mSound->playTrack(SOUNDID_PLAYERFIRE3);

                    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE1);
                    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE2);
                }
                break;
            }
        } else {
            theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE1);
            theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE2);
            theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE3);
        }
    }

    runMissiles();

    mDrawSheild = false;

    if (mSheildTimer > 0)
        --mSheildTimer;

    if (mSheildTimer < 60) {
        mDrawSheild = (mSheildTimer / 6) & 1;
    } else
        mDrawSheild = true;

    if (mSheildTimer == 60) {
        theGame->mSound->playTrack(SOUNDID_SHIELDSLOST);
    }

    if (mDrawSheild) {
        attractor::Attractor* att = theGame->mAttractors->getAttractor();
        if (att) {
            att->strength = 100;
            att->radius = 3.5;
            att->pos = mPos;
            att->enabled = true;
            att->attractsParticles = false;
        }
    }

    entity::run();
}

void player::indicating()
{
    if (--mStateTimer <= 0) {
        setState(ENTITY_STATE_INACTIVE);
    }
}

void player::runMissiles()
{
    for (std::size_t i = 0; i < missiles.size(); i++) {
        entityPlayerMissile* missile = &missiles[i];
        if (missile->getEnabled()) {
            switch (missile->getState()) {
            case entity::ENTITY_STATE_SPAWN_TRANSITION:
                missile->spawnTransition();
                break;
            case entity::ENTITY_STATE_SPAWNING:
                missile->spawn();
                break;
            case entity::ENTITY_STATE_RUN_TRANSITION:
                missile->runTransition();
                break;
            case entity::ENTITY_STATE_RUNNING:
                missile->run();
                break;
            case entity::ENTITY_STATE_DESTROY_TRANSITION:
                missile->destroyTransition();
                break;
            case entity::ENTITY_STATE_DESTROYED:
                missile->destroy();
                break;
            default:
                printf("Unhandled missile state\n");
                break;
            }
        }
    }
}

void player::draw()
{
    // Draw the missiles
    for (std::size_t i = 0; i < missiles.size(); i++) {
        entityPlayerMissile* missile = &missiles[i];
        if (missile->getEnabled())
            missile->draw();
    }

    if (getEnabled()) {
        // Draw the shields
        if (mDrawSheild) {
            float delta_theta = 0.05;
            float r = 2.5;

            glColor4f(mPen.r, mPen.g, mPen.b, mPen.a);

            glBegin(GL_LINE_LOOP);

            for (float angle = 0; angle < 2 * PI; angle += delta_theta)
                glVertex3f(mPos.x + (r * get_cos(angle)), mPos.y + (r * get_sin(angle)), 0);

            glEnd();
        }
    }

    if (getState() != entity::ENTITY_STATE_DESTROYED)
        entity::draw();
}

void player::spawnTransition()
{
    mSheildTimer = PLAYER_SHEILD_TIME;

    float angle = getAngle();
    entity::spawnTransition();
    setAngle(angle);

    mDrawSheild = true;

    theGame->mSound->playTrack(SOUNDID_PLAYERSPAWN);
}

void player::spawn()
{
    entity::spawn();

    mDrawSheild = true;

    // Rez-up grid distortion

    float b = (float)mStateTimer / mSpawnTime;
    b = 1 - b;

    attractor::Attractor* att = theGame->mAttractors->getAttractor();
    if (att) {
        att->strength = 20;
        att->radius = 28 * b;
        att->pos = mPos;
        att->enabled = true;
        att->attractsParticles = true;
    }
    att = theGame->mAttractors->getAttractor();
    if (att) {
        att->strength = -20;
        att->radius = 30 * b;
        att->pos = mPos;
        att->enabled = true;
        att->attractsParticles = true;
    }
}

void player::firePattern1(const Point3d& fireAngle, const Point3d& playerSpeed)
{
    if (--mFiringTimer <= 0) {
        mFiringTimer = 6;

        // Find an unused missile
        entityPlayerMissile* missile1 = nullptr;
        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile1 = &missiles[i];
                missile1->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile1->mType = 0;
                missile1->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        entityPlayerMissile* missile2 = nullptr;
        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile2 = &missiles[i];
                missile2->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile2->mType = 0;
                missile2->mPlayerSource = mPlayerAssignment;
                break;
            }
        }

        if (missile1 && missile2) {
            float angle = mathutils::calculate2dAngle(Point3d(0, 0, 0), fireAngle) + mathutils::DegreesToRads(90);

            float speed = .7;
            float spread = .4;
            float missileAngle1 = (angle + spread);
            float missileAngle2 = (angle - spread);

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(2, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileAngle1);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle);

                missile1->setPos(missilePos);
                missile1->setAngle(angle - mathutils::DegreesToRads(90));
                missile1->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile1->mVelocity = speed;
            }

            // ***************************************

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(2, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileAngle2);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle);

                missile2->setPos(missilePos);
                missile2->setAngle(angle - mathutils::DegreesToRads(90));
                missile2->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile2->mVelocity = speed;
            }

            // ***************************************
        }
    }
}

void player::firePattern2(const Point3d& fireAngle, const Point3d& playerSpeed)
{
    if (--mFiringTimer <= 0) {
        static bool alternate = true;
        alternate = !alternate;
        if (alternate)
            mFiringTimer = 4;
        else
            mFiringTimer = 1;

        entityPlayerMissile* missile1 = nullptr;
        entityPlayerMissile* missile2 = nullptr;
        entityPlayerMissile* missile3 = nullptr;

        // Find an unused missile
        if (alternate) {
            for (std::size_t i = 0; i < missiles.size(); i++) {
                if (!missiles[i].getEnabled()) {
                    missile1 = &missiles[i];
                    missile1->setState(ENTITY_STATE_SPAWN_TRANSITION);
                    missile1->mType = 1;
                    missile1->mPlayerSource = mPlayerAssignment;
                    break;
                }
            }
            for (std::size_t i = 0; i < missiles.size(); i++) {
                if (!missiles[i].getEnabled()) {
                    missile2 = &missiles[i];
                    missile2->setState(ENTITY_STATE_SPAWN_TRANSITION);
                    missile2->mType = 1;
                    missile2->mPlayerSource = mPlayerAssignment;
                    break;
                }
            }
        } else {
            for (std::size_t i = 0; i < missiles.size(); i++) {
                if (!missiles[i].getEnabled()) {
                    missile3 = &missiles[i];
                    missile3->setState(ENTITY_STATE_SPAWN_TRANSITION);
                    missile3->mType = 1;
                    missile3->mPlayerSource = mPlayerAssignment;
                    break;
                }
            }
        }

        float angle = mathutils::calculate2dAngle(Point3d(0, 0, 0), fireAngle) + mathutils::DegreesToRads(90);
        float speed = 1.2;

        if (missile3) {
            Point3d missilePos;
            Point3d missileSpeedVector(speed, 0, 0);
            Point3d missileOffsetVector(.5, 0, 0);

            missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, angle);
            missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle);

            missile3->setPos(missilePos);
            missile3->setAngle(angle - mathutils::DegreesToRads(90));
            missile3->setSpeed(missileSpeedVector + (playerSpeed * .5));
            missile3->mVelocity = speed;
        } else if (missile1 && missile2) {
            float spread = .8;
            float missileAngle1 = (angle + spread);
            float missileAngle2 = (angle - spread);

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(.5, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileAngle1);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle + .06);

                missile1->setPos(missilePos);
                missile1->setAngle(angle - mathutils::DegreesToRads(90));
                missile1->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile1->mVelocity = speed;
            }

            // ***************************************

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(.5, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileAngle2);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle - .06);

                missile2->setPos(missilePos);
                missile2->setAngle(angle - mathutils::DegreesToRads(90));
                missile2->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile2->mVelocity = speed;
            }
        }
    }
}

void player::firePattern3(const Point3d& fireAngle, const Point3d& playerSpeed)
{
    if (--mFiringTimer <= 0) {
        mFiringTimer = 7;

        // Find 5 unused missiles
        entityPlayerMissile* missile0 = nullptr;
        entityPlayerMissile* missile1 = nullptr;
        entityPlayerMissile* missile2 = nullptr;
        entityPlayerMissile* missile3 = nullptr;
        entityPlayerMissile* missile4 = nullptr;

        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile0 = &missiles[i];
                missile0->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile0->mType = 2;
                missile0->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile1 = &missiles[i];
                missile1->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile1->mType = 2;
                missile1->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile2 = &missiles[i];
                missile2->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile2->mType = 2;
                missile2->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile3 = &missiles[i];
                missile3->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile3->mType = 2;
                missile3->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (std::size_t i = 0; i < missiles.size(); i++) {
            if (!missiles[i].getEnabled()) {
                missile4 = &missiles[i];
                missile4->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile4->mType = 2;
                missile4->mPlayerSource = mPlayerAssignment;
                break;
            }
        }

        if (missile0 && missile1 && missile2 && missile3 && missile4) {
            float angle = mathutils::calculate2dAngle(Point3d(0, 0, 0), fireAngle) + mathutils::DegreesToRads(90);

            float speed = .9;
            float start1 = .1;
            float start2 = .15;
            float spread1 = .05;
            float spread2 = .09;
            float missileStart1 = (angle + start1);
            float missileStart2 = (angle - start1);
            float missileStart3 = (angle + start2);
            float missileStart4 = (angle - start2);
            float missileSpread1 = (angle + spread1);
            float missileSpread2 = (angle - spread1);
            float missileSpread3 = (angle + spread2);
            float missileSpread4 = (angle - spread2);

            // Missile 0 just fires out straight
            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(2, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, angle);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle);

                missile0->setPos(missilePos);
                missile0->setAngle(angle - mathutils::DegreesToRads(90));
                missile0->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile0->mVelocity = speed;
            }

            // Missile 1 and 2 fire at a medium spread but aimed not far off from center

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(2, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileStart1);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, missileSpread1);

                missile1->setPos(missilePos);
                missile1->setAngle(angle - mathutils::DegreesToRads(90));
                missile1->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile1->mVelocity = speed;
            }

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(2, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileStart2);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, missileSpread2);

                missile2->setPos(missilePos);
                missile2->setAngle(angle - mathutils::DegreesToRads(90));
                missile2->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile2->mVelocity = speed;
            }

            // Missile 3 and 4 fire at a far spread

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(1, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileStart3);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, missileSpread3);

                missile3->setPos(missilePos);
                missile3->setAngle(angle - mathutils::DegreesToRads(90));
                missile3->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile3->mVelocity = speed;
            }

            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(1, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileStart4);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, missileSpread4);

                missile4->setPos(missilePos);
                missile4->setAngle(angle - mathutils::DegreesToRads(90));
                missile4->setSpeed(missileSpeedVector + (playerSpeed * .5));
                missile4->mVelocity = speed;
            }
        }
    }
}

void player::destroyTransition()
{
    entity::destroyTransition();

    mStateTimer = mDestroyTime;

    // Reset the multipler stuff
    mMultiplier = 1;
    mKillCounter = 0;

    attractor::Attractor* att = theGame->mAttractors->getAttractor();
    if (att) {
        att->strength = 200;
        att->radius = 30;
        att->pos = mPos;
        att->enabled = true;
        att->attractsParticles = true;
    }

    // Throw out some particles
    Point3d pos(this->mPos);
    Point3d angle(0, 0, 0);
    float speed = 2.0;
    float spread = 2 * PI;
    int num = 500;
    int timeToLive = 200;
    vector::pen pen = mPen;
    pen.r *= 1.2;
    pen.g *= 1.2;
    pen.b *= 1.2;
    pen.a = 200;
    pen.lineRadius = 5;
    theGame->mParticles->emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, true, true, .97, true);

    setState(ENTITY_STATE_DESTROYED);

    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE1);
    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE2);
    theGame->mSound->stopTrack(SOUNDID_PLAYERFIRE3);

    if (getNumLives() <= 1) {
        theGame->endGame();
    } else
        theGame->mSound->playTrack(SOUNDID_PLAYERHIT);
}

void player::destroy()
{
    if (--mStateTimer <= 0) {
        setState(ENTITY_STATE_INACTIVE);
    }
}

void player::addKillAtLocation(int points, Point3d pos)
{
    int pointsEarned = points * mMultiplier;
    mScore += pointsEarned;
    mWeaponCounter += pointsEarned;
    mBombCounter += pointsEarned;
    mLifeCounter += pointsEarned;

    /*
    For every enemy you destroy you receive that enemy's base score times your
    multiplier. Your multiplier begins at x1. After destroying 25 enemies in
    one life your multiplier becomes x2. At 50 enemies x3, and so on, up to
    x10 with 2,000 enemies destroyed in one life. If you die your multiplier
    returns to zero.

    Every 10,000 points there is a chance your main weapon will change. Which
    weapon you receive is random, but if you receive the same weapon you are
    currently using then it's like nothing happened.

    You will receive an extra life every 75,000 point interval on the score.

    You will receive an extra bomb every 100,000 point interval on the score.
    */

    if (mWeaponCounter >= 10000) {
        mWeaponCounter = 0;
        switchWeapons();
    }

    if (mBombCounter >= 100000) {
        mBombCounter = 0;
        addBomb();
        theGame->mSound->playTrack(SOUNDID_EXTRABOMB);
    }
    if (mLifeCounter >= 75000) {
        mLifeCounter = 0;
        addLife();
        theGame->mSound->playTrack(SOUNDID_EXTRALIFE);
    }

    bool showMultiplier = false;

    ++mKillCounter;
    if (mKillCounter >= 25) {
        // Increment the multiplier and display a message
        mKillCounter = 0;
        if (mMultiplier <= 5) {
            ++mMultiplier;
            showMultiplier = true;
        }
    }

    vector::pen pen = this->getFontPen();

    if (showMultiplier) {
        // Show the multiplier message
        char message[256];
        sprintf(message, "Multiplier x%d", mMultiplier);
        game::showMessageAtLocation(message, pos, pen);
        theGame->mSound->playTrack(SOUNDID_MULTIPLIERADVANCE);
    } else {
        // Just display the point value
        char message[128];
        sprintf(message, "%d", pointsEarned);
        game::showMessageAtLocation(message, pos, pen);
    }
}

int player::getNumLives()
{
    if (theGame->numPlayers() == 1)
        return mNumLives;
    else
        return game::m2PlayerNumLives;
}

int player::getNumBombs()
{
    if (theGame->numPlayers() == 1)
        return mNumBombs;
    else
        return game::m2PlayerNumBombs;
}

void player::addLife()
{
    if (theGame->numPlayers() == 1)
        ++mNumLives;
    else
        ++game::m2PlayerNumLives;
}

void player::takeLife()
{
    if (theGame->numPlayers() == 1)
        --mNumLives;
    else
        --game::m2PlayerNumLives;

    if (mNumLives < 0)
        mNumLives = 0;

    if (game::m2PlayerNumLives < 0)
        game::m2PlayerNumLives = 0;
}

void player::addBomb()
{
    if (theGame->numPlayers() == 1)
        ++mNumBombs;
    // No bombs on 2 player game
}

void player::takeBomb()
{
    if (theGame->numPlayers() == 1)
        --mNumBombs;
    // No bombs on 2 player game
}

void player::addPointsNoMultiplier(int points)
{
    int pointsEarned = points;
    mScore += pointsEarned;
    mBombCounter += pointsEarned;
    mLifeCounter += pointsEarned;
}

void player::switchWeapons()
{
    if (mCurrentWeapon == 0) {
        mCurrentWeapon = 1;
    } else {
        mCurrentWeapon = (mathutils::frandFrom0To1() * 100) < 50 ? 1 : 2;
    }
}
