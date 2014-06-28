#include "player.h"
#include "game.h"

const int player::mMaxMissiles = 500;

player::player()
    : entity()
{
    mDrawSheild = FALSE;

    mScoreValue = 0;

    mSpawnTime = 40;

    mSheildTimer = PLAYER_SHEILD_TIME;

    mJoined = FALSE;

    mDestroyTime = 40;

    mExhaustSpreadIndex = mathutils::randFromTo(0,1);

    setState(ENTITY_STATE_INACTIVE);

    // Create our missiles
    missiles = new entityPlayerMissile[mMaxMissiles];
    for (int i=0; i<mMaxMissiles; i++)
    {
        entityPlayerMissile* missile = &missiles[i];
        missile->setEnabled(false);
    }

    initPlayerForGame();
}

player::~player()
{
	delete [] missiles;
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
    for (int i=0; i<mMaxMissiles; i++)
    {
        entityPlayerMissile* missile = &missiles[i];
        missile->setEnabled(false);
    }

    setState(entity::ENTITY_STATE_SPAWN_TRANSITION);

    mJoined = TRUE;
}

void player::deinitPlayerForGame()
{
    mJoined = TRUE;
}

void player::run()
{
    if (getEnabled())
    {
        // Read the trigger
        if (theGame.mGameType == game::GAMETYPE_SINGLEPLAYER)
        {
            bool trigger = game::mControls.getTriggerButton(mPlayerAssignment);
            if (trigger)
            {
                if (getNumBombs() > 0 && mBombInterimTimer <= 0)
                {
                    // Fire off a bomb
                    takeBomb();
                    mBombInterimTimer = 50;

                    game::mBomb.startBomb(mPos, 1, 6, 2, 200, vector::pen(1,1,1,.3,4));
                    game::mSound.playTrack(SOUNDID_BOMB);
                    theGame.startBomb();
                }
            }
            if (mBombInterimTimer>0)
            {
                --mBombInterimTimer;
            }
        }

        Point3d playerSpeed(0,0,0);

        // Move the player
        Point3d leftStick = game::mControls.getLeftStick(mPlayerAssignment);
        float distance = mathutils::calculate2dDistance(Point3d(0,0,0), leftStick);
        if (distance > .1)
        {
            //
            // The movement stick is being used
            //

            if (distance > .6)
            {
                distance = 1;
            }
            else distance = .5;

            float angle = mathutils::calculate2dAngle(Point3d(0,0,0), leftStick) + mathutils::DegreesToRads(90);

            angle -= mathutils::DegreesToRads(90);

            // Rotate to the correct angle
			angle = mathutils::wrapRadians(angle);
            float currentAngle = mathutils::wrapRadians(this->getAngle());
            float diff = mathutils::diffAngles(angle, currentAngle);
            currentAngle += diff * .2;
            this->setAngle(currentAngle);

            // Move
            Point3d thrust(distance*0.75, 0, 0);
            thrust = mathutils::rotate2dPoint(thrust, currentAngle+mathutils::DegreesToRads(90));
            playerSpeed = thrust;
            this->setPos(this->getPos() + thrust);

            // Emit exhaust particles
            Point3d exhaustAngle = currentAngle + mathutils::DegreesToRads(180);
            Point3d exhaustOffset;

            float speed = .8; // .5
            int num = 1;
            int timeToLive = 200;
            vector::pen pen = this->getExhaustPen();
            pen.a = 20;

            // Main stream
            {
                float spread = .1;
                exhaustOffset = mathutils::rotate2dPoint(Point3d(0, -2, 0), currentAngle);
                exhaustOffset += getPos();
                game::mParticles.emitter(&exhaustOffset, &exhaustAngle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .93, FALSE);
            }
            // First swirl
            {
                exhaustOffset = mathutils::rotate2dPoint(Point3d(0, -3, 0), currentAngle + (get_sin(mExhaustSpreadIndex) * .3));
                exhaustOffset += getPos();

                float spread = 0;
                game::mParticles.emitter(&exhaustOffset, &exhaustAngle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .93, FALSE);
            }
            // Second swirl
            {
                exhaustOffset = mathutils::rotate2dPoint(Point3d(0, -3, 0), currentAngle + (get_sin(-mExhaustSpreadIndex) * .3));
                exhaustOffset += getPos();

                float spread = 0;
                game::mParticles.emitter(&exhaustOffset, &exhaustAngle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .93, FALSE);
            }
            mExhaustSpreadIndex += .18;

            if (!game::mSound.isTrackPlaying(SOUNDID_PLAYERTHRUST))
                game::mSound.playTrack(SOUNDID_PLAYERTHRUST);
        }
        else
        {
            game::mSound.stopTrack(SOUNDID_PLAYERTHRUST);
        }

        // Firing
        Point3d rightStick = game::mControls.getRightStick(mPlayerAssignment);
        distance = mathutils::calculate2dDistance(Point3d(0,0,0), rightStick);
        if (distance > .1)
        {
            //
            // The firing stick is being used
            //

            switch (mCurrentWeapon)
            {
                case 0:
                    firePattern1(rightStick, playerSpeed);
                    {
                        if (!game::mSound.isTrackPlaying(SOUNDID_PLAYERFIRE1))
                            game::mSound.playTrack(SOUNDID_PLAYERFIRE1);

                        game::mSound.stopTrack(SOUNDID_PLAYERFIRE2);
                        game::mSound.stopTrack(SOUNDID_PLAYERFIRE3);
                    }
                    break;
                case 1:
                    firePattern2(rightStick, playerSpeed);
                    {
                        if (!game::mSound.isTrackPlaying(SOUNDID_PLAYERFIRE2))
                            game::mSound.playTrack(SOUNDID_PLAYERFIRE2);

                        game::mSound.stopTrack(SOUNDID_PLAYERFIRE1);
                        game::mSound.stopTrack(SOUNDID_PLAYERFIRE3);
                    }
                    break;
                case 2:
                    firePattern3(rightStick, playerSpeed);
                    {
                        if (!game::mSound.isTrackPlaying(SOUNDID_PLAYERFIRE3))
                            game::mSound.playTrack(SOUNDID_PLAYERFIRE3);

                        game::mSound.stopTrack(SOUNDID_PLAYERFIRE1);
                        game::mSound.stopTrack(SOUNDID_PLAYERFIRE2);
                    }
                    break;
            }
        }
        else
        {
            game::mSound.stopTrack(SOUNDID_PLAYERFIRE1);
            game::mSound.stopTrack(SOUNDID_PLAYERFIRE2);
            game::mSound.stopTrack(SOUNDID_PLAYERFIRE3);
        }
    }

    runMissiles();

    mDrawSheild = FALSE;

    if (mSheildTimer > 0)
        --mSheildTimer;

    if (mSheildTimer < 60)
    {
        mDrawSheild = (mSheildTimer/6) & 1;
    }
    else mDrawSheild = TRUE;

    if (mSheildTimer == 60)
    {
        game::mSound.playTrack(SOUNDID_SHIELDSLOST);
    }

    if (mDrawSheild)
    {
        attractor::Attractor* att = game::mAttractors.getAttractor();
        if (att)
        {
            att->strength = 100;
            att->radius = 3.5;
            att->pos = mPos;
            att->enabled = TRUE;
            att->attractsParticles = FALSE;
        }
    }

    entity::run();
}

void player::indicating()
{
    if (--mStateTimer <= 0)
    {
        setState(ENTITY_STATE_INACTIVE);
    }
}

void player::runMissiles()
{
    for (int i=0; i<mMaxMissiles; i++)
    {
        entityPlayerMissile* missile = &missiles[i];
        if (missile->getEnabled())
        {
            switch(missile->getState())
            {
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
            }
        }
    }
}


void player::draw()
{
    // Draw the missiles
    for (int i=0; i<mMaxMissiles; i++)
    {
        entityPlayerMissile* missile = &missiles[i];
        if (missile->getEnabled())
            missile->draw();
    }

    if (getEnabled())
    {
        // Draw the shields
        if (mDrawSheild)
        {
            float delta_theta = 0.05;
            float r = 2.5;

            glColor4f(mPen.r, mPen.g, mPen.b, mPen.a);

            glBegin(GL_LINE_LOOP);

            for (float angle = 0; angle < 2*PI; angle += delta_theta )
                glVertex3f( mPos.x + (r*get_cos(angle)), mPos.y + (r*get_sin(angle)), 0 );

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

    mDrawSheild = TRUE;

    game::mSound.playTrack(SOUNDID_PLAYERSPAWN);
}

void player::spawn()
{
    entity::spawn();

    mDrawSheild = TRUE;

    // Rez-up grid distortion

    attractor::Attractor* att = game::mAttractors.getAttractor();
    if (att)
    {
        att->strength = -.5;
        att->radius = 100;
        att->pos = mPos;
        att->enabled = TRUE;
        att->attractsParticles = TRUE;
    }
}

void player::firePattern1(const Point3d& fireAngle, const Point3d& playerSpeed)
{
    if (--mFiringTimer <= 0)
    {
        mFiringTimer = 6;

        // Find an unused missile
        entityPlayerMissile* missile1 = NULL;
        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile1 = &missiles[i];
                missile1->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile1->mType = 0;
                missile1->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        entityPlayerMissile* missile2 = NULL;
        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile2 = &missiles[i];
                missile2->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile2->mType = 0;
                missile2->mPlayerSource = mPlayerAssignment;
                break;
            }
        }

        if (missile1 && missile2)
        {
            float angle = mathutils::calculate2dAngle(Point3d(0,0,0), fireAngle) + mathutils::DegreesToRads(90);

            float speed = .9;
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
    if (--mFiringTimer <= 0)
    {
        static BOOL alternate = TRUE;
        alternate = !alternate;
        if (alternate)
            mFiringTimer = 4;
        else
            mFiringTimer = 1;

        entityPlayerMissile* missile1 = NULL;
        entityPlayerMissile* missile2 = NULL;
        entityPlayerMissile* missile3 = NULL;

        // Find an unused missile
        if (alternate)
        {
            for (int i=0; i<mMaxMissiles; i++)
            {
                if (!missiles[i].getEnabled())
                {
                    missile1 = &missiles[i];
                    missile1->setState(ENTITY_STATE_SPAWN_TRANSITION);
                    missile1->mType = 1;
                    missile1->mPlayerSource = mPlayerAssignment;
                    break;
                }
            }
            for (int i=0; i<mMaxMissiles; i++)
            {
                if (!missiles[i].getEnabled())
                {
                    missile2 = &missiles[i];
                    missile2->setState(ENTITY_STATE_SPAWN_TRANSITION);
                    missile2->mType = 1;
                    missile2->mPlayerSource = mPlayerAssignment;
                    break;
                }
            }
        }
        else
        {
            for (int i=0; i<mMaxMissiles; i++)
            {
                if (!missiles[i].getEnabled())
                {
                    missile3 = &missiles[i];
                    missile3->setState(ENTITY_STATE_SPAWN_TRANSITION);
                    missile3->mType = 1;
                    missile3->mPlayerSource = mPlayerAssignment;
                    break;
                }
            }
        }

        float angle = mathutils::calculate2dAngle(Point3d(0,0,0), fireAngle) + mathutils::DegreesToRads(90);
        float speed = 1.2;

        if (missile3)
        {
            Point3d missilePos;
            Point3d missileSpeedVector(speed, 0, 0);
            Point3d missileOffsetVector(.5, 0, 0);

            missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, angle);
            missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle);

            missile3->setPos(missilePos);
            missile3->setAngle(angle - mathutils::DegreesToRads(90));
            missile3->setSpeed(missileSpeedVector + (playerSpeed * .5));
            missile3->mVelocity = speed;
        }
        else if (missile1 && missile2)
        {
            float spread = .8;
            float missileAngle1 = (angle + spread);
            float missileAngle2 = (angle - spread);
            
            {
                Point3d missilePos;
                Point3d missileSpeedVector(speed, 0, 0);
                Point3d missileOffsetVector(.5, 0, 0);

                missilePos = this->getPos() + mathutils::rotate2dPoint(missileOffsetVector, missileAngle1);
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle+.06);

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
                missileSpeedVector = mathutils::rotate2dPoint(missileSpeedVector, angle-.06);

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
    if (--mFiringTimer <= 0)
    {
        mFiringTimer = 7;

        // Find 5 unused missiles
        entityPlayerMissile* missile0 = NULL;
        entityPlayerMissile* missile1 = NULL;
        entityPlayerMissile* missile2 = NULL;
        entityPlayerMissile* missile3 = NULL;
        entityPlayerMissile* missile4 = NULL;


        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile0 = &missiles[i];
                missile0->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile0->mType = 2;
                missile0->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile1 = &missiles[i];
                missile1->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile1->mType = 2;
                missile1->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile2 = &missiles[i];
                missile2->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile2->mType = 2;
                missile2->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile3 = &missiles[i];
                missile3->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile3->mType = 2;
                missile3->mPlayerSource = mPlayerAssignment;
                break;
            }
        }
        for (int i=0; i<mMaxMissiles; i++)
        {
            if (!missiles[i].getEnabled())
            {
                missile4 = &missiles[i];
                missile4->setState(ENTITY_STATE_SPAWN_TRANSITION);
                missile4->mType = 2;
                missile4->mPlayerSource = mPlayerAssignment;
                break;
            }
        }

        if (missile0 && missile1 && missile2 && missile3 && missile4)
        {
            float angle = mathutils::calculate2dAngle(Point3d(0,0,0), fireAngle) + mathutils::DegreesToRads(90);

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

    // Reset the multipler
    mMultiplier = 1;

    attractor::Attractor* att = game::mAttractors.getAttractor();
    if (att)
    {
        att->strength = 200;
        att->radius = 30;
        att->pos = mPos;
        att->enabled = TRUE;
        att->attractsParticles = TRUE;
    }

    // Throw out some particles
    Point3d pos(this->mPos);
    Point3d angle(0,0,0);
    float speed = 2;
    float spread = 2*PI;
    int num = 200;
    int timeToLive = 300;
    vector::pen pen = mPen;
    pen.r *= 1.2;
    pen.g *= 1.2;
    pen.b *= 1.2;
    pen.a = .8;
    pen.lineRadius=5;
    game::mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .98, TRUE);

    setState(ENTITY_STATE_DESTROYED);

    game::mSound.stopTrack(SOUNDID_PLAYERFIRE1);
    game::mSound.stopTrack(SOUNDID_PLAYERFIRE2);
    game::mSound.stopTrack(SOUNDID_PLAYERFIRE3);

    if (getNumLives() <= 1)
    {
        theGame.endGame();
    }
    else
        game::mSound.playTrack(SOUNDID_PLAYERHIT);
}

void player::destroy()
{
    if (--mStateTimer <= 0)
    {
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

    if (mWeaponCounter >= 10000)
    {
        mWeaponCounter = 0;
        switchWeapons();
    }

    if (mBombCounter >= 100000)
    {
        mBombCounter = 0;
        addBomb();
        game::mSound.playTrack(SOUNDID_EXTRABOMB);
    }
    if (mLifeCounter >= 75000)
    {
        mLifeCounter = 0;
        addLife();
        game::mSound.playTrack(SOUNDID_EXTRALIFE);
    }

    bool showMultiplier = false;

    ++mKillCounter;
    if (mKillCounter >= 25)
    {
        // Increment the multiplier and display a message
        mKillCounter = 0;
        if (mMultiplier < 10)
        {
            ++mMultiplier;
            showMultiplier = true;
        }
    }

    vector::pen pen = this->getFontPen();

    if (showMultiplier)
    {
        // Show the multiplier message
        char message[256];
        sprintf(message, "Multiplier x%d", mMultiplier);
        game::showMessageAtLocation(message, pos, pen);
        game::mSound.playTrack(SOUNDID_MULTIPLIERADVANCE);
    }
    else
    {
        // Just display the point value
        char message[128];
        sprintf(message, "%d", pointsEarned);
        game::showMessageAtLocation(message, pos, pen);
    }
}

int player::getNumLives()
{
    if (theGame.numPlayers() == 1)
        return mNumLives;
    else return game::m2PlayerNumLives;
}

int player::getNumBombs()
{
    if (theGame.numPlayers() == 1)
        return mNumBombs;
    else return game::m2PlayerNumBombs;
}

void player::addLife()
{
    if (theGame.numPlayers() == 1)
        ++mNumLives;
    else ++game::m2PlayerNumLives;
}

void player::takeLife()
{
    if (theGame.numPlayers() == 1)
        --mNumLives;
    else --game::m2PlayerNumLives;

    if (mNumLives < 0)
        mNumLives = 0;

    if (game::m2PlayerNumLives < 0)
        game::m2PlayerNumLives = 0;
}

void player::addBomb()
{
    if (theGame.numPlayers() == 1)
        ++mNumBombs;
    // No bombs on 2 player game
}

void player::takeBomb()
{
    if (theGame.numPlayers() == 1)
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
    if (mCurrentWeapon == 0)
    {
        mCurrentWeapon = 1;
    }
    else
    {
        mCurrentWeapon = (mathutils::frandFrom0To1() * 100) < 50 ? 1 : 2;
    }
}


