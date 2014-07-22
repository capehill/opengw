#include "entityblackhole.h"
#include "game.h"
#include "sincos.h"

entityBlackHole::entityBlackHole()
{
    mScale = 1.5;
    mRadius = 2.5;

    mEdgeBounce = TRUE;

    mScoreValue = 50;

    mPen = vector::pen(1, .5, .5, .7, 4);

    mType = ENTITY_TYPE_BLACKHOLE;
    setState(ENTITY_STATE_INACTIVE);

    mActivated = FALSE;
    mDestroyTime = 0;
    mAnimationIndex = 0;
    mAnimationSpeed = 0;
    mAnimationDepth = 0;
    mAttractorStrengthIndex = 0;
    mStrength = 1;
    mBalance = 0;
    mBalanceRate = 0;

    mGridPullIndex = mathutils::frandFrom0To1();

    mHumLoopSoundId = -1;

    // This entity doesn't use a model
}

void entityBlackHole::runTransition()
{
    entity::runTransition();
}

void entityBlackHole::run()
{
    if (mActivated)
    {
        mAnimationSpeed = mStrength * .5;
    }
    else
    {
        mAnimationSpeed = .5;
    }
    mAnimationDepth = /*mStrength **/ .2;

    mSpeed = mathutils::clamp2dVector(mSpeed, .2);
    mSpeed *= .96;

    mDrift = mathutils::clamp2dVector(mDrift, .8);

    mHumSpeedTarget = (mStrength*mStrength)/2;
    if (mHumSpeed < mHumSpeedTarget)
        mHumSpeed += .01;
    else if (mHumSpeed > mHumSpeedTarget)
        mHumSpeed -= .01;

    if (mActivated)
    {
        mStrength += .0003;

        if (mHumLoopSoundId == -1)
            mHumLoopSoundId = game::mSound.playTrackGroup(SOUNDID_GRAVITYWELL_HUMLOOPA, SOUNDID_GRAVITYWELL_HUMLOOPF);
        if (mHumLoopSoundId != -1)
        game::mSound.setTrackSpeed(mHumLoopSoundId, mHumSpeed+.2);

        if (mStrength > 2.2)
        {
            if (!game::mSound.isTrackPlaying(SOUNDID_GRAVITYWELLALERT))
                game::mSound.playTrack(SOUNDID_GRAVITYWELLALERT);
        }
        if (mStrength > 2.3)
        {
            // Explode and spawn a bunch of protons
            setState(ENTITY_STATE_DESTROY_TRANSITION);
            game::mSound.playTrack(SOUNDID_GRAVITYWELLEXPLODE);

            for (int i=0; i<20; i++)
            {
                entity* enemy = game::mEnemies.getUnusedEnemyOfType(entity::ENTITY_TYPE_PROTON);
                if (enemy)
                {
                    const float distance = mathutils::frandFrom0To1() * 10;
                    float angle = mathutils::frandFrom0To1() * (2*PI);
                    Point3d spawnPoint(distance, 0, 0);
                    spawnPoint = mathutils::rotate2dPoint(spawnPoint, angle);

                    enemy->setDrift(Point3d(spawnPoint.x*100, spawnPoint.y*100, 0));
                    enemy->setPos(spawnPoint + mPos);
                    enemy->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
                }
            }

            return;
        }

        if (mBalance > 0)
            mBalanceRate -= .01;
        else if (mBalance < 0)
            mBalanceRate += .01;

        mBalanceRate *= .95;
        mBalance += mBalanceRate;
        mBalance *= .95;

        // Distort the grid

        const float distance = 2;

        float dir = (mathutils::frandFrom0To1() * 100) < 10 ? -1 : 1;

        attractor::Attractor* att = game::mAttractors.getAttractor();
        if (att)
        {
            float s = sin(mGridPullIndex);

            if (fabs(s) > .7)
            {
                mGridPullIndex += 0.2f;
            }
            else
            {
                mGridPullIndex += 0.04f;
            }

            if (s > 0) s *= .5;
            s = ((s+1)/2) + .5;

//            mGridPullIndex += 0.04f;
//            s = ((s+1)/2) + 1;


            att->strength = s * -30;

            att->radius = 6;

            att->pos = mPos;
            att->enabled = TRUE;
            att->attractsParticles = TRUE;
        }
    }

    // Seek the player

    float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos());
    Point3d moveVector(1, 0, 0);
    moveVector = mathutils::rotate2dPoint(moveVector, angle);
    mSpeed += moveVector * .002;

    mSpeed *= .98;

    mAnimationIndex+=mAnimationSpeed;

    entity::run();


    // Keep it on the grid

    const float leftEdge = getRadius();
    const float bottomEdge = getRadius();
    const float rightEdge = (theGame.mGrid.extentX() - getRadius())-1;
    const float topEdge = (theGame.mGrid.extentY() - getRadius())-1;

    if (mPos.x < leftEdge)
    {
        mPos.x = leftEdge;
        mSpeed.x = -mSpeed.x;
    }
    else if (mPos.x > rightEdge)
    {
        mPos.x = rightEdge;
        mSpeed.x = -mSpeed.x;
    }
    if (mPos.y < bottomEdge)
    {
        mPos.y = bottomEdge;
        mSpeed.y = -mSpeed.y;
    }
    else if (mPos.y > topEdge)
    {
        mPos.y = topEdge;
        mSpeed.y = -mSpeed.y;
    }

}

void entityBlackHole::spawnTransition()
{
    entity::spawnTransition();

    mActivated = FALSE;
    mRadius = 2.5;
    mStrength = 1.5;
    mBalance = 0;
    mBalanceRate = 0;
    mFeedCount = 0;

    mDestroyTime = 0;
    mAnimationIndex = 0;
    mAnimationSpeed = 0;
    mAnimationDepth = 0;
    mAttractorStrengthIndex = 0;
    mBalance = 0;

    mPoints = 150;

    mHumSpeed = (mStrength*mStrength)/2;
    mHumSpeedTarget = mHumSpeed;
    mHumLoopSoundId = -1;

    game::mSound.playTrack(SOUNDID_ENEMYSPAWN5);
}

void entityBlackHole::destroyTransition()
{
    // Here we actually get destroyed
    entity::destroyTransition();

    if (mHumLoopSoundId != -1)
        game::mSound.stopTrack(mHumLoopSoundId);
    mHumLoopSoundId = -1;

    // Throw out some particles
    for (int i=0; i<100; i++)
    {
        Point3d pos(this->mPos);
        Point3d angle(0,0,0);
        float speed = ((float)i/200) * 1000;
        float spread = 2*PI;
        int num = 1;
        int timeToLive = mathutils::frandFrom0To1() * 300;
        vector::pen pen = mPen;
        pen.lineRadius=5;
        game::mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive);
    }

    attractor::Attractor* att = game::mAttractors.getAttractor();
    if (att)
    {
        att->strength = 100;
        att->radius = 50;
        att->pos = mPos;
        att->enabled = TRUE;
        att->attractsParticles = FALSE;
    }
}

void entityBlackHole::indicateTransition()
{
    entity::indicateTransition();

    if (mHumLoopSoundId != -1)
        game::mSound.stopTrack(mHumLoopSoundId);
    mHumLoopSoundId = -1;
}

void entityBlackHole::hit(entity* aEntity)
{
    game::mSound.playTrack(SOUNDID_GRAVITYWELLHIT);

    entityPlayerMissile* missile = dynamic_cast<entityPlayerMissile*>(aEntity);
    if (missile)
    {
        if (mActivated)
        {
            mStrength *= .98;

            if (mStrength < .7)
            {
                // Destroy
                setState(ENTITY_STATE_DESTROY_TRANSITION);
                game::mSound.playTrack(SOUNDID_GRAVITYWELLDESTROYED);

                entityPlayerMissile* missile = dynamic_cast<entityPlayerMissile*>(aEntity);
                if (missile)
                {
                    if (mPoints)
                    {
                        Point3d pos = getPos();

                        // Add points and display them at the destruction point
                        if (missile->mPlayerSource == 0)
                            game::mPlayers.mPlayer1->addKillAtLocation(mPoints, pos);
                        else if (missile->mPlayerSource == 1)
                            game::mPlayers.mPlayer2->addKillAtLocation(mPoints, pos);
                    }
                }
            }
        }
        else
        {
            mActivated = TRUE;

            if (theGame.mGameMode == game::GAMEMODE_PLAYING)
            {
                if (mHumLoopSoundId == -1)
                    mHumLoopSoundId = game::mSound.playTrackGroup(SOUNDID_GRAVITYWELL_HUMLOOPA, SOUNDID_GRAVITYWELL_HUMLOOPF);
            }

            mBalance = 1.6;
            mBalanceRate = 0;

/*
            // Distrupt the grid at the activation point
            attractor::Attractor* att = game::mAttractors.getAttractor();
            if (att)
            {
                att->strength = 20;
                att->radius = 20;
                att->pos = mPos;
                att->enabled = TRUE;
                att->attractsParticles = FALSE;
            }
*/
        }

        float r;
        if (mActivated)
        {
            r = mRadius + (get_sin(mAnimationIndex)*mAnimationDepth);
            r *= mStrength + (mBalance*.1);
        }
        else
        {
            r = mRadius;
        }

/*
        {
            Point3d angle(0, 0, 0);
            float speed = 2;
            float spread = (2*PI);
            int num = mathutils::frandFrom0To1() * 30;
            int timeToLive = 500;
            vector::pen pen = mPen;
            pen.r = mathutils::frandFrom0To1();
            pen.g = mathutils::frandFrom0To1();
            pen.b = mathutils::frandFrom0To1();
            pen.a = .4;
            pen.lineRadius=5;
            game::mParticles.emitter(&mPos, &angle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .92, TRUE);
        }
*/
        for (int i=0; i<360; i++)
        {
            if (mathutils::frandFrom0To1() * 100 < 10)
            {
                float ang = mathutils::DegreesToRads(i);
                Point3d pos = Point3d(0, r+1, 0);
                pos = mathutils::rotate2dPoint(pos, ang);
                pos += mPos;
                Point3d angle(0,ang,0);

                float speed = 30;
                float spread = 0;
                int num = 1;
                int timeToLive = mathutils::frandFrom0To1() * 200;
                vector::pen pen = mPen;
                pen.r = 1;//mathutils::frandFrom0To1() + .5;
                pen.g = 1;//mathutils::frandFrom0To1() + .5;
                pen.b = 1;//mathutils::frandFrom0To1() + .5;
                pen.a = .5;
                pen.lineRadius=5;
                game::mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, FALSE, TRUE, .95);
            }
        }

        mBalance -= .2;
    }
    else if (aEntity && aEntity->getType() == entity::ENTITY_TYPE_BLACKHOLE)
    {
        // This code can probably go away
        // This code can probably go away
        // This code can probably go away

        mActivated = TRUE;

        mBalance = 2;
        mBalanceRate = 0;

        // Distrupt the grid at the activation point
        attractor::Attractor* att = game::mAttractors.getAttractor();
        if (att)
        {
            att->strength = 20;
            att->radius = 20;
            att->pos = mPos;
            att->enabled = TRUE;
            att->attractsParticles = FALSE;
        }

        // This code can probably go away
        // This code can probably go away
        // This code can probably go away
    }
    else
    {
        // It must be a bomb
        this->destroyTransition();
    }
}

entity* entityBlackHole::hitTest(const Point3d& pos, float radius)
{
    float distance = mathutils::calculate2dDistance(pos, mPos);
    float resultRadius = radius + getRadius();
    if (distance < resultRadius)
    {
        return this;
    }
    return NULL;
}

void entityBlackHole::draw()
{
    if (this->getState() == entity::ENTITY_STATE_INDICATING)
    {
        if (((int)(mStateTimer/10)) & 1)
        {
        }
        else return;
    }

    if (this->getEnabled())
    {
        if (getState() == ENTITY_STATE_SPAWNING)
        {
            vector::pen pen = mPen;
            float radius = mRadius;

            float inc = 1.0f / mSpawnTime;
            float progress = mStateTimer * inc;

            // *********************************************

            progress = 1-progress;

            float a = progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            mPen.a = a;

            mRadius = radius * progress * 1;
            drawRing();

            // *********************************************

            progress = progress + .25;

            a = 1-progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            mPen.a = a;

            mRadius = radius * progress * 4;
            drawRing();

            // *********************************************

            progress = progress + .25;

            a = 1-progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            mPen.a = a;

            mRadius = radius * progress * 7;
            drawRing();

            // *********************************************

            // Restore stuff
            mRadius = radius;
            mPen = pen;
        }

        drawRing();

    }
}

void entityBlackHole::drawRing()
{
    BOOL activated = (this->getState() == entity::ENTITY_STATE_INDICATING) ? false : mActivated;

    float delta_theta = 0.05;

    float r;
    if (activated)
    {
        r = mRadius + (get_sin(mAnimationIndex)*mAnimationDepth);
        r *= mStrength + (mBalance*.1);
    }
    else
    {
        r = mRadius;
    }

    // Enable if you want the black holes to appear as "holes" in the grid
#if 0
    if (activated && (mState != entity::ENTITY_STATE_SPAWNING) && (scene::mPass != scene::RENDERPASS_PRIMARY))
    {
        glDisable(GL_BLEND);

        glColor4f(1, .5, .5, 1);

        glBegin(GL_TRIANGLE_FAN);

        glVertex3f( mPos.x, mPos.y, 0 );

        float c = r*1.5;

        for (float angle = 0; angle < 2.01*PI; angle += delta_theta )
            glVertex3f( mPos.x + (c*get_cos(angle)), mPos.y + (c*get_sin(angle)), 0 );

        glEnd();

	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    if (activated && (mState != entity::ENTITY_STATE_SPAWNING))
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0, 0, 0, 1);

        glBegin(GL_TRIANGLE_FAN);

        glVertex3f( mPos.x, mPos.y, 0 );

        float c = r;

        for (float angle = 0; angle < 2.01*PI; angle += delta_theta )
            glVertex3f( mPos.x + (c*get_cos(angle)), mPos.y + (c*get_sin(angle)), 0 );

        glEnd();

	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
#endif

    if ((mState != entity::ENTITY_STATE_SPAWNING) && (scene::mPass != scene::RENDERPASS_PRIMARY))
    {
	    glDisable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glColor4f(.3, .05, .05, 1);
        glLineWidth(6);

        glBegin(GL_LINE_LOOP);

        float c = r+.2;

        for (float angle = 0; angle < 2*PI; angle += delta_theta )
            glVertex3f( mPos.x + (c*get_cos(angle)), mPos.y + (c*get_sin(angle)), 0 );

        glEnd();


        glBegin(GL_LINE_LOOP);

        c = r;

        for (float angle = 0; angle < 2*PI; angle += delta_theta )
            glVertex3f( mPos.x + (c*get_cos(angle)), mPos.y + (c*get_sin(angle)), 0 );

        glEnd();


	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    float c =(get_sin(mAnimationIndex)*mAnimationDepth);

    if (activated)
    {
        if (scene::mPass == scene::RENDERPASS_PRIMARY)
        {
            glColor4f(1,1,1,1);
        }
        else
        {
            glColor4f(mPen.r, .25, .25, 1);
        }
    }
    else if (mState != entity::ENTITY_STATE_SPAWNING)
    {
        //glColor4f((mPen.r*c)+.5, (mPen.g*c)+.25, (mPen.b*c)+.25, mPen.a);

        if (scene::mPass == scene::RENDERPASS_PRIMARY)
        {
            glColor4f(mPen.r, mPen.g, mPen.b, (mPen.a*c)+.5);
        }
        else
        {
            glColor4f(mPen.r, .25, .25, 1);
        }
    }
    else
    {
        glColor4f(mPen.r, mPen.g, mPen.b, mPen.a);
    }

    if (activated && (mState != entity::ENTITY_STATE_SPAWNING))
    {
        glLineWidth(mPen.lineRadius);
    }
    else
    {
        glLineWidth(mPen.lineRadius);
    }

    if (!theGame.mEnemySmoothing)
    {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_MULTISAMPLE);
    }

    glBegin(GL_LINE_LOOP);

    for (float angle = 0; angle < 2*PI; angle += delta_theta )
    {
        glVertex3f( mPos.x + (r*get_cos(angle)), mPos.y + (r*get_sin(angle)), 0 );
    }

    glEnd();

    if (!theGame.mEnemySmoothing)
    {
        glDisable(GL_MULTISAMPLE);
        glDisable(GL_LINE_SMOOTH);
    }
}

void entityBlackHole::feed(int points)
{
    mPoints += points * 2;
    mStrength += .06;
    mBalanceRate += .14;
    ++mFeedCount;
}

const float entityBlackHole::getRadius() const
{
    float r;
    if (mActivated)
    {
        r = mRadius + (get_sin(mAnimationIndex)*mAnimationDepth);
        r *= mStrength + (mBalance*.1);
        r *= 2;
    }
    else
    {
        r = mRadius;
    }

    return r;
}



