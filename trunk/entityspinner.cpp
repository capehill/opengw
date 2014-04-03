#include "entitySpinner.h"
#include "game.h"
#include "sincos.h"

entitySpinner::entitySpinner()
    : entity()
{
    mScale = 1.6;
    mRadius = 2.5;

    mScoreValue = 100;
//    mScoreValue = 50;

    mType = ENTITY_TYPE_SPINNER;
    setState(ENTITY_STATE_INACTIVE);

    mAnimationIndex = 0;

    mPen = vector::pen(1, .5, 1, .7, 12);

    int i = 0;

    mModel.mNumVertex = 5;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, 0);
    mModel.mVertexList[i++] = Point3d(-1, 1);
    mModel.mVertexList[i++] = Point3d(1, 1);
    mModel.mVertexList[i++] = Point3d(1, -1);
    mModel.mVertexList[i++] = Point3d(-1, -1);

    i = 0;

    mModel.mNumEdges = 8;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 0;
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
}

void entitySpinner::run()
{
    if (this->getEnabled())
    {
        // Seek the player

        float angle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos());
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        mSpeed += moveVector * .2;
        mSpeed = mathutils::clamp2dVector(mSpeed, .4 * mAggression);
        mSpeed *= .9;

        // Run animation
        mAnimationIndex += .12; // .07
        mAngle = (get_sin(mAnimationIndex)) * .5;
    }
    entity::run();
}

void entitySpinner::destroyTransition()
{
    // Spawn 2 tiny spinners here at 90 degrees to the player
    Point3d pos = mPos;
    entity::destroyTransition();


    // Spawn them at opposing right angles to the player
    Point3d spawnPoint(12,0,0);
    float angleToPlayer = mathutils::calculate2dAngle(pos, game::mPlayers.mPlayer1->getPos());
    spawnPoint = mathutils::rotate2dPoint(spawnPoint, angleToPlayer + mathutils::DegreesToRads(90));

    entity* enemy = game::mEnemies.getUnusedEnemyOfType(entity::ENTITY_TYPE_TINYSPINNER);
    if (enemy)
    {
        enemy->setPos(spawnPoint + pos);
        enemy->setSpeed(spawnPoint * .2);
        enemy->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
    }

    spawnPoint = mathutils::rotate2dPoint(spawnPoint, angleToPlayer - mathutils::DegreesToRads(90));

    enemy = game::mEnemies.getUnusedEnemyOfType(entity::ENTITY_TYPE_TINYSPINNER);
    if (enemy)
    {
        enemy->setPos(spawnPoint + pos);
        enemy->setSpeed(spawnPoint * .2);
        enemy->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
    }
}

void entitySpinner::hit(entity* aEntity)
{
    if (aEntity)
    {
        entityPlayerMissile* missile = dynamic_cast<entityPlayerMissile*>(aEntity);
        if (missile)
        {
            entity::hit(aEntity);
            return;
        }
    }

    // If it's not a missile don't spawn tiny spinners

    entity::destroyTransition();
}

void entitySpinner::spawnTransition()
{
    entity::spawnTransition();
    game::mSound.playTrack(SOUNDID_ENEMYSPAWN2);
}








