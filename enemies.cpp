#include "enemies.h"
#include "game.h"
#include "entityblackhole.h"

#include <cstdio>

static int idxWandererStart;
static int idxWandererEnd;

static int idxGruntStart;
static int idxGruntEnd;

static int idxSpinnerStart;
static int idxSpinnerEnd;

static int idxTinySpinnerStart;
static int idxTinySpinnerEnd;

static int idxWeaverStart;
static int idxWeaverEnd;

static int idxSnakeStart;
static int idxSnakeEnd;

static int idxBlackHoleStart;
static int idxBlackHoleEnd;

static int idxRepulsorStart;
static int idxRepulsorEnd;

static int idxMayflyStart;
static int idxMayflyEnd;

static int idxProtonStart;
static int idxProtonEnd;




enemies::enemies(const game& gameRef) : mGame(gameRef)
{
    int entity = 0;

    // Wanderers
    {
		idxWandererStart = entity;

        for (int i=0; i<numEnemyWanderer; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_WANDERER, gameRef);
        }

		idxWandererEnd = entity-1;
	}
    // Grunts
    {
		idxGruntStart = entity;

        for (int i=0; i<numEnemyGrunt; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_GRUNT, gameRef);
        }

		idxGruntEnd = entity-1;
    }
    // Spinners
    {
		idxSpinnerStart = entity;

        for (int i=0; i<numEnemySpinner; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_SPINNER, gameRef);
        }

		idxSpinnerEnd = entity-1;
    }
    // Tiny Spinners
    {
		idxTinySpinnerStart = entity;

        for (int i=0; i<numEnemyTinySpinner; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_TINYSPINNER, gameRef);
        }

		idxTinySpinnerEnd = entity-1;
    }
    // Weavers
    {
		idxWeaverStart = entity;

        for (int i=0; i<numEnemyWeaver; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_WEAVER, gameRef);
        }

		idxWeaverEnd = entity-1;
    }
    // Snakes
    {
		idxSnakeStart = entity;

        for (int i=0; i<numEnemySnake; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_SNAKE, gameRef);
        }

		idxSnakeEnd = entity-1;
    }
    // Black holes
    {
		idxBlackHoleStart = entity;

        for (int i=0; i<numEnemyBlackHole; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_BLACKHOLE, gameRef);
        }

		idxBlackHoleEnd = entity-1;
    }
    // Repulsors
    {
		idxRepulsorStart = entity;

        for (int i=0; i<numEnemyRepulsor; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_REPULSOR, gameRef);
        }

		idxRepulsorEnd = entity-1;
    }
    // Mayflies
    {
		idxMayflyStart = entity;

        for (int i=0; i<numEnemyMayfly; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_MAYFLY, gameRef);
        }

		idxMayflyEnd = entity-1;
    }
    // Protons
    {
		idxProtonStart = entity;

        for (int i=0; i<numEnemyProton; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_PROTON, gameRef);
        }

		idxProtonEnd = entity-1;
    }

#ifdef USE_SDL
	printf("Num enemies created = %d\n", entity);
#else
    TCHAR s[256];
    wsprintf(s, L"Num enemies created = %d\n", entity);
    OutputDebugString(s);
#endif

    // Fill the rest of the list with empty entries
    for (int i=entity; i<NUM_ENEMIES; i++)
    {
        mEnemies[i] = entity::createEntity(entity::ENTITY_TYPE_UNDEF, gameRef);
    }

    for (int i=0; i<NUM_LINES; i++)
    {
        mLines[i] = static_cast<entityLine*>(entity::createEntity(entity::ENTITY_TYPE_LINE, gameRef));
    }
}

enemies::~enemies()
{
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        delete mEnemies[i];
    }

    for (int i=0; i<NUM_LINES; i++)
    {
        delete mLines[i];
    }
}

void enemies::run()
{
    // Run each enemy
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        switch(mEnemies[i]->getState())
        {
            case entity::ENTITY_STATE_SPAWN_TRANSITION:
                mEnemies[i]->spawnTransition();
                {
                    // NOT SURE I LIKE THIS SINCE IT'S NOT ACCURATE TO THE ORIGINAL GAME
                    // Add a little "poof" to the grid whereever an enemy spawns
/*
                    attractor::Attractor* att = game::mAttractors.getAttractor();
                    if (att)
                    {
                        att->strength = 5;
                        att->radius = 15;
                        att->pos = mEnemies[i]->getPos();
                        att->enabled = TRUE;
                        att->attractsParticles = TRUE;
                    }
*/
                }
                break;
            case entity::ENTITY_STATE_SPAWNING:
                mEnemies[i]->spawn();
                break;
            case entity::ENTITY_STATE_RUN_TRANSITION:
                mEnemies[i]->runTransition();
                break;
            case entity::ENTITY_STATE_RUNNING:
                mEnemies[i]->run();
                break;
            case entity::ENTITY_STATE_DESTROY_TRANSITION:
                mEnemies[i]->destroyTransition();
                break;
            case entity::ENTITY_STATE_DESTROYED:
                mEnemies[i]->destroy();
                break;
            case entity::ENTITY_STATE_INDICATE_TRANSITION:
                mEnemies[i]->indicateTransition();
                break;
            case entity::ENTITY_STATE_INDICATING:
                mEnemies[i]->indicating();
                break;
        }
    }

    // Run each line
    for (int i=0; i<NUM_LINES; i++)
    {
        switch(mLines[i]->getState())
        {
            case entity::ENTITY_STATE_SPAWN_TRANSITION:
                mLines[i]->spawnTransition();
                break;
            case entity::ENTITY_STATE_SPAWNING:
                mLines[i]->spawn();
                break;
            case entity::ENTITY_STATE_RUN_TRANSITION:
                mLines[i]->runTransition();
                break;
            case entity::ENTITY_STATE_RUNNING:
                mLines[i]->run();
                break;
            case entity::ENTITY_STATE_DESTROY_TRANSITION:
                mLines[i]->destroyTransition();
                break;
            case entity::ENTITY_STATE_DESTROYED:
                mLines[i]->destroy();
                break;
        }
    }

    // Keep the entities separated
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if (mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING)
        {
            for (int j=0; j<NUM_ENEMIES; j++)
            {
                if (mEnemies[j]->getState() == entity::ENTITY_STATE_RUNNING)
                {
                    if (j != i)
                    {
                        entity* e1 = mEnemies[i];
                        entity* e2 = mEnemies[j];
                        float distance = mathutils::calculate2dDistance(e1->getPos(), e2->getPos());
                        float totalRadius = e1->getRadius() + e2->getRadius();
                        if (distance < totalRadius)
                        {
                            // Nudge each away from each other
                            float angle = mathutils::calculate2dAngle(e2->getPos(), e1->getPos());
                            Point3d vector(1,0,0);
                            vector = mathutils::rotate2dPoint(vector, angle);
                            if (e1->getType() == entity::ENTITY_TYPE_BLACKHOLE)
                            {
                                entityBlackHole* blackHole = dynamic_cast<entityBlackHole*>(e1);
                                if (blackHole)
                                {
                                    if (blackHole->mActivated)
                                    {
                                        e1->setSpeed(e1->getSpeed() + vector * .2);
                                    }
                                }
                            }
                            else
                            {
                                e1->setPos(e1->getPos() + vector * .02);
                            }
                        }
                    }
                }
            }
        }
    }

}

void enemies::draw()
{
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if (mEnemies[i]->getEnabled())
        {
            mEnemies[i]->draw();
        }
    }
    for (int i=0; i<NUM_LINES; i++)
    {
        if (mLines[i]->getEnabled())
        {
            mLines[i]->draw();
        }
    }
}

entity* enemies::hitTestEnemiesAtPosition(const Point3d& point, float radius, bool includeSpawning/*=false*/)
{
    includeSpawning = false; // I don't like this anymore

    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if ((mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING)
            || (includeSpawning && (mEnemies[i]->getState() == entity::ENTITY_STATE_SPAWNING) && (mEnemies[i]->getType() != entity::ENTITY_TYPE_BLACKHOLE)))
        {
            entity* e = mEnemies[i]->hitTest(point, radius);
            if (e)
            {
                return e;
            }
        }
    }

    return NULL;
}

int enemies::getNumActiveEnemiesOfType(const entity::EntityType& type)
{
	int idxStart { 0 };
	int idxEnd { 0 };

	switch (type)
	{
		case entity::ENTITY_TYPE_WANDERER:
			idxStart = idxWandererStart;
			idxEnd = idxWandererEnd;
			break;
		case entity::ENTITY_TYPE_GRUNT:
			idxStart = idxGruntStart;
			idxEnd = idxGruntEnd;
			break;
		case entity::ENTITY_TYPE_SPINNER:
			idxStart = idxSpinnerStart;
			idxEnd = idxSpinnerEnd;
			break;
		case entity::ENTITY_TYPE_TINYSPINNER:
			idxStart = idxTinySpinnerStart;
			idxEnd = idxTinySpinnerEnd;
			break;
		case entity::ENTITY_TYPE_WEAVER:
			idxStart = idxWeaverStart;
			idxEnd = idxWeaverEnd;
			break;
		case entity::ENTITY_TYPE_SNAKE:
			idxStart = idxSnakeStart;
			idxEnd = idxSnakeEnd;
			break;
		case entity::ENTITY_TYPE_BLACKHOLE:
			idxStart = idxBlackHoleStart;
			idxEnd = idxBlackHoleEnd;
			break;
		case entity::ENTITY_TYPE_REPULSOR:
			idxStart = idxRepulsorStart;
			idxEnd = idxRepulsorEnd;
			break;
		case entity::ENTITY_TYPE_MAYFLY:
			idxStart = idxMayflyStart;
			idxEnd = idxMayflyEnd;
			break;
		case entity::ENTITY_TYPE_PROTON:
			idxStart = idxProtonStart;
			idxEnd = idxProtonEnd;
			break;
	}

    int count = 0;
    for (int i=idxStart; i<=idxEnd; i++)
    {
        if (mEnemies[i]->getState() != entity::ENTITY_STATE_INACTIVE)
        {
            ++count;
        }
    }

    return count;
}

entity* enemies::getUnusedEnemyOfType(const entity::EntityType& type)
{
	int idxStart { 0 };
	int idxEnd { 0 };

	switch (type)
	{
		case entity::ENTITY_TYPE_WANDERER:
			idxStart = idxWandererStart;
			idxEnd = idxWandererEnd;
			break;
		case entity::ENTITY_TYPE_GRUNT:
			idxStart = idxGruntStart;
			idxEnd = idxGruntEnd;
			break;
		case entity::ENTITY_TYPE_SPINNER:
			idxStart = idxSpinnerStart;
			idxEnd = idxSpinnerEnd;
			break;
		case entity::ENTITY_TYPE_TINYSPINNER:
			idxStart = idxTinySpinnerStart;
			idxEnd = idxTinySpinnerEnd;
			break;
		case entity::ENTITY_TYPE_WEAVER:
			idxStart = idxWeaverStart;
			idxEnd = idxWeaverEnd;
			break;
		case entity::ENTITY_TYPE_SNAKE:
			idxStart = idxSnakeStart;
			idxEnd = idxSnakeEnd;
			break;
		case entity::ENTITY_TYPE_BLACKHOLE:
			idxStart = idxBlackHoleStart;
			idxEnd = idxBlackHoleEnd;
			break;
		case entity::ENTITY_TYPE_REPULSOR:
			idxStart = idxRepulsorStart;
			idxEnd = idxRepulsorEnd;
			break;
		case entity::ENTITY_TYPE_MAYFLY:
			idxStart = idxMayflyStart;
			idxEnd = idxMayflyEnd;
			break;
		case entity::ENTITY_TYPE_PROTON:
			idxStart = idxProtonStart;
			idxEnd = idxProtonEnd;
			break;
	}

    for (int i=idxStart; i<=idxEnd; i++)
    {
        if (mEnemies[i]->getState() == entity::ENTITY_STATE_INACTIVE)
        {
            return mEnemies[i];
        }
    }

    return NULL;
}

entityLine* enemies::getUnusedLine()
{
    for (int i=0; i<NUM_LINES; i++)
    {
        if (mLines[i]->getState() == entity::ENTITY_STATE_INACTIVE)
        {
            return mLines[i];
        }
    }

    return NULL;
}

void enemies::explodeEntity(entity& e)
{
    model* m = e.getModel();

    Point3d objectPos(0,0,0);
    m->mMatrix.TransformVertex(objectPos, &objectPos);

    int numEdges = m->mNumEdges;
    for (int i=0; i<numEdges; i++)
    {
        entityLine* line = getUnusedLine();
        if (line)
        {
            vector::pen pen = e.getPen();
            pen.r *= 1.5;
            pen.g *= 1.5;
            pen.b *= 1.5;
//            if (pen.r > 1) pen.r = 1;
//            if (pen.g > 1) pen.g = 1;
//            if (pen.b > 1) pen.b = 1;
            pen.a = 5;
            line->setPen(pen);

            Point3d from = m->mVertexList[m->mEdgeList[i].from];
            Point3d to = m->mVertexList[m->mEdgeList[i].to];
            Point3d midpoint((from.x + to.x) / 2, (from.y + to.y) / 2, 0);

            Point3d offset(midpoint.x, midpoint.y);

            from.x -= offset.x;
            from.y -= offset.y;
            to.x -= offset.x;
            to.y -= offset.y;

            line->addEndpoints(from, to);
            line->setPos(objectPos + midpoint);
            line->setScale(e.getScale());
            line->setAngle(e.getAngle());

            line->setRotationRate((mathutils::frandFrom0To1() * .8) - .4);

            float angle = mathutils::calculate2dAngle(Point3d(0,0,0), midpoint);

            float variation = (mathutils::frandFrom0To1() * 2) - 1;

            Point3d speedVector(0,.5,0);
            speedVector = mathutils::rotate2dPoint(speedVector, angle + variation);

            speedVector += e.getDrift() * .1;

            line->setSpeed(speedVector);
            line->setState(entity::ENTITY_STATE_SPAWN_TRANSITION);
        }
    }
}


void enemies::disableAllEnemies()
{
    for (int i=0; i<NUM_ENEMIES; i++)
    {
        if (mEnemies[i]->getState() != entity::ENTITY_STATE_INDICATING)
        {
            mEnemies[i]->setState(entity::ENTITY_STATE_INACTIVE);
            mEnemies[i]->incGenId();
        }
    }
}

void enemies::disableAllLines()
{
    for (int i=0; i<NUM_LINES; i++)
    {
        mLines[i]->setState(entity::ENTITY_STATE_INACTIVE);
        mLines[i]->incGenId();
    }
}

