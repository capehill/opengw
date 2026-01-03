#include "defines.hpp"
#include "entityrepulsor.hpp"
#include "enemies.hpp"
#include "entityPlayer1.hpp"
#include "entityline.hpp"
#include "game.hpp"
#include "grid.hpp"
#include "players.hpp"

class entityRepulsorShieldLine : public entity
{
  public:
    entityRepulsorShieldLine()
    {
        mScale = .12;
        mRadius = 2.6;

        mValue = 1;

        mType = ENTITY_TYPE_REPULSOR_SHIELD;
        setState(ENTITY_STATE_INACTIVE);

        mEdgeBounce = false;
        mGridBound = false;

        mPen = vector::pen(.96, .51, .35, .7, 12);

        int i = 0;

        // The shield
        mModel.mVertexList.resize(2);
        mModel.mVertexList[i++] = Point3d(-12, 20);
        mModel.mVertexList[i++] = Point3d(12, 20);

        i = 0;

        mModel.mEdgeList.resize(1);
        mModel.mEdgeList[i].from = 0;
        mModel.mEdgeList[i++].to = 1;
    }

    void hit(entity* /*aEntity*/)
    {
        // Do nothing and don't call the base class method (shield lines are invinsible)
    }

    void destroyTransition()
    {
        setState(ENTITY_STATE_DESTROYED);
        mStateTimer = mDestroyTime;

        ++mGenId;

        if (mValue >= 1) {
            // Create line entities for the shield lines
            entityLine* line = theGame->mEnemies->getUnusedLine();
            if (line) {
                line->setEdgeBounce(false);
                line->setGridBound(false);

                vector::pen pen = mPen;
                pen.a = 1;
                line->setPen(pen);

                // Here we add the simulated "speed" of the shield line moving forward to the final speed of the line
                Point3d newSpeed = mSpeed;
                Point3d thrust(.4, 0, 0);
                thrust = mathutils::rotate2dPoint(thrust, mathutils::wrapRadians(mAngle + mathutils::DegreesToRads(90)));
                newSpeed += thrust;

                line->addEndpoints(mLine.from, mLine.to);
                line->setPos(mPos);
                line->setScale(mScale);
                line->setAngle(mAngle);
                line->setSpeed(newSpeed);
                line->setRotationRate(mRotationRate);
                line->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
            }
        }

        mValue = 0;
    }

    void run()
    {
        // Animate the shield line
        const Point3d p1(-8, 20);
        const Point3d p2(8, 20);

        mLine.from.x = p1.x * mValue; //(mValue * ((mValue*.4) + .6));
        mLine.from.y = p1.y * mValue;
        mLine.to.x = p2.x * mValue; //(mValue * ((mValue*.4) + .6));
        mLine.to.y = p2.y * mValue;

        mModel.mVertexList[0].x = mLine.from.x;
        mModel.mVertexList[0].y = mLine.from.y;
        mModel.mVertexList[1].x = mLine.to.x;
        mModel.mVertexList[1].y = mLine.to.y;

        mValue += .1;
        if (mValue >= 6)
            mValue -= 6;
    }

    void draw()
    {
        if (mValue >= 1) {
            entity::draw();
        }
    }

    float mValue;
    vector::vector mLine;
};

entityRepulsor::entityRepulsor(const game& gameRef)
    : entity(), mGame(gameRef)
{
    mScale = .12;
    mRadius = 2.6;

    mScoreValue = 100;

    mEdgeBounce = false;
    mGridBound = false;

    mAIState = State_Aiming;
    mTimer = 0;
    mShieldsEnabled = false;

    mLoopSoundId = -1;

    mType = ENTITY_TYPE_REPULSOR;
    setState(ENTITY_STATE_INACTIVE);

    mPen = vector::pen(.96, .51, .35, .7, 12);

    int i = 0;

    mModel.mVertexList.resize(12);
    mModel.mVertexList[i++] = Point3d(8, -1);
    mModel.mVertexList[i++] = Point3d(8, 8);
    mModel.mVertexList[i++] = Point3d(11.5, 12);
    mModel.mVertexList[i++] = Point3d(15, 8);
    mModel.mVertexList[i++] = Point3d(15, -5);
    mModel.mVertexList[i++] = Point3d(5, -14);
    mModel.mVertexList[i++] = Point3d(-5, -14);
    mModel.mVertexList[i++] = Point3d(-15, -5);
    mModel.mVertexList[i++] = Point3d(-15, 8);
    mModel.mVertexList[i++] = Point3d(-11.5, 12);
    mModel.mVertexList[i++] = Point3d(-8, 8);
    mModel.mVertexList[i++] = Point3d(-8, -1);

    i = 0;

    mModel.mEdgeList.resize(16);
    mModel.mEdgeList[i].from = 0;
    mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1;
    mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2;
    mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3;
    mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4;
    mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5;
    mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6;
    mModel.mEdgeList[i++].to = 7;
    mModel.mEdgeList[i].from = 7;
    mModel.mEdgeList[i++].to = 8;
    mModel.mEdgeList[i].from = 8;
    mModel.mEdgeList[i++].to = 9;
    mModel.mEdgeList[i].from = 9;
    mModel.mEdgeList[i++].to = 10;
    mModel.mEdgeList[i].from = 10;
    mModel.mEdgeList[i++].to = 11;
    mModel.mEdgeList[i].from = 11;
    mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0;
    mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 11;
    mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 8;
    mModel.mEdgeList[i++].to = 10;
    mModel.mEdgeList[i].from = 1;
    mModel.mEdgeList[i++].to = 3;

    mShield1 = std::make_unique<entityRepulsorShieldLine>();
    mShield2 = std::make_unique<entityRepulsorShieldLine>();
    mShield3 = std::make_unique<entityRepulsorShieldLine>();
}

void entityRepulsor::run()
{
    if (this->getEnabled()) {
        // Check for missiles around us to see if we need to activate our shield lines
        bool enable = false;
        const auto& missiles = mGame.getPlayer1()->missiles;
        for (std::size_t i = 0; (i < missiles.size()) && !enable; i++) {
            const entityPlayerMissile* missile = &missiles[i];
            if (missile->getEnabled()) {
                // Test this missile to see if it's close to us
                float distance = mathutils::calculate2dDistance(missile->getPos(), mPos);
                if (distance < 40) {
                    enable = true;
                }
            }
        }

        mShieldsEnabled = enable;

        if (mShieldsEnabled) {
            // Run the shield lines
            mShield1->run();
            mShield2->run();
            mShield3->run();
        } else {
            mShield1->mValue = 0;
            mShield2->mValue = -2;
            mShield3->mValue = -4;
        }

        Point3d targetPos = mGame.mPlayers->getPlayerClosestToPosition(mPos)->getPos();
        float angle = mathutils::calculate2dAngle(mPos, targetPos) + mathutils::DegreesToRads(-90);
        float angleDiff = mathutils::diffAngles(mAngle, angle);

        // Do the sound loop
        if (mAIState == State_Charging) {
            if (mLoopSoundId == -1)
                mLoopSoundId = theGame->mSound->playTrackGroup(SOUNDID_REPULSORA, SOUNDID_REPULSORD);
        } else {
            if (mLoopSoundId != -1)
                theGame->mSound->stopTrack(mLoopSoundId);
            mLoopSoundId = -1;
        }

        if (mAIState == State_Charging) {
            // Accelerate towards the players
            Point3d speed = Point3d(0, 1, 0);
            speed = mathutils::rotate2dPoint(speed, mAngle);
            mSpeed += speed * .07;

            if (fabs(angleDiff) > mathutils::DegreesToRads(60)) {
                if (--mTimer <= 0) {
                    mAIState = State_Thinking;
                    mTimer = 10;
                }
            }

            // Rotate towards the player
            mAngle -= angleDiff * .035;
        } else if (mAIState == State_Aiming) {
            // Rotate towards the player
            if (fabs(angleDiff) < .2) {
                // Start charging
                mAIState = State_Charging;
                mTimer = 20;
            } else {
                mAngle -= angleDiff * .05;
            }
        } else // mAIState == State_Thinking
        {
            if (--mTimer <= 0) {
                mAIState = State_Aiming;
            }
        }

        mSpeed *= .95;
    }
    entity::run();

    // If we hit the grid, stop
    Point3d hitPoint;
    Point3d speed = mSpeed;
    if (theGame->mGrid->hitTest(mPos, mRadius, &hitPoint, &speed)) {
        mPos = hitPoint;
        mSpeed = speed * .5;
    }

    // Align the shield lines with ourselves and run them
    mShield1->setEnabled(true);
    mShield1->setPos(mPos);
    mShield1->setScale(mScale);
    mShield1->setAngle(mAngle);
    mShield1->setSpeed(mSpeed);
    mShield1->setRotationRate(mRotationRate);
    mShield1->setDrift(mDrift);
    mShield1->run();

    mShield2->setEnabled(true);
    mShield2->setPos(mPos);
    mShield2->setScale(mScale);
    mShield2->setAngle(mAngle);
    mShield2->setSpeed(mSpeed);
    mShield2->setRotationRate(mRotationRate);
    mShield2->setDrift(mDrift);
    mShield2->run();

    mShield3->setEnabled(true);
    mShield3->setPos(mPos);
    mShield3->setScale(mScale);
    mShield3->setAngle(mAngle);
    mShield3->setSpeed(mSpeed);
    mShield3->setRotationRate(mRotationRate);
    mShield3->setDrift(mDrift);
    mShield3->run();
}

void entityRepulsor::spawnTransition()
{
    entity::spawnTransition();

    mSpeed = Point3d(0, 0, 0);
    mRotationRate = 0;

    mShield1->mValue = 0;
    mShield2->mValue = -2;
    mShield3->mValue = -4;

    // Pick a random direction
    mAngle = mathutils::frandFrom0To1() * (2 * PI);

    mAIState = State_Aiming;

    theGame->mSound->playTrack(SOUNDID_ENEMYSPAWN1);
}

void entityRepulsor::draw()
{
    entity::draw();

    // Draw the shield lines
    if (mShieldsEnabled && (mState == entity::ENTITY_STATE_RUNNING)) {
        mShield1->draw();
        mShield2->draw();
        mShield3->draw();
    }
}

void entityRepulsor::indicateTransition()
{
    entity::indicateTransition();

    if (mLoopSoundId != -1)
        theGame->mSound->stopTrack(mLoopSoundId);
    mLoopSoundId = -1;
}

void entityRepulsor::destroyTransition()
{
    if (mLoopSoundId != -1)
        theGame->mSound->stopTrack(mLoopSoundId);
    mLoopSoundId = -1;

    entity::destroyTransition();
    mShield1->destroyTransition();
    mShield2->destroyTransition();
    mShield3->destroyTransition();
}

void entityRepulsor::repelEntity(entity* e)
{
    if ((mState == entity::ENTITY_STATE_RUNNING) && mShieldsEnabled) {
        // Here we just see if the missile is within a certain distance and within a certain range of angles
        float distance = mathutils::calculate2dDistance(mPos, e->getPos());
        if (distance < 25) {
            float angle = mathutils::calculate2dAngle(mPos, e->getPos());
            float diff = mathutils::diffAngles(mAngle + mathutils::DegreesToRads(90), angle);
            if (fabs(diff) < .75) {
                // Push the missile away
                const float amount = 200;
                Point3d moveVector(0, diff > 0 ? amount : -amount, 0);
                moveVector = mathutils::rotate2dPoint(moveVector, angle + mathutils::DegreesToRads(180));
                moveVector = mathutils::clamp2dVector(moveVector, .06);
                e->setDrift(e->getDrift() + moveVector);
            }
        }
    }
}
