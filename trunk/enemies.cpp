#include "enemies.h"
#include "game.h"
#include "entityblackhole.h"


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

static int idxMayflyStart;
static int idxMayflyEnd;

static int idxProtonStart;
static int idxProtonEnd;

static int idxGeomSmallStart;
static int idxGeomSmallEnd;

static int idxGeomMediumStart;
static int idxGeomMediumEnd;

static int idxGeomLargeStart;
static int idxGeomLargeEnd;


enemies::enemies()
{
    int entity = 0;

    // Wanderers
    {
		idxWandererStart = entity;

        int num = 10;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_WANDERER);
        }

		idxWandererEnd = entity-1;
	}
    // Grunts
    {
		idxGruntStart = entity;

        int num = 100;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_GRUNT);
        }

		idxGruntEnd = entity-1;
    }
    // Spinners
    {
		idxSpinnerStart = entity;

        int num = 25;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_SPINNER);
        }

		idxSpinnerEnd = entity-1;
    }
    // Tiny Spinners
    {
		idxTinySpinnerStart = entity;

        int num = 50;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_TINYSPINNER);
        }

		idxTinySpinnerEnd = entity-1;
    }
    // Weavers
    {
		idxWeaverStart = entity;

        int num = 100;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_WEAVER);
        }

		idxWeaverEnd = entity-1;
    }
    // Snakes
    {
		idxSnakeStart = entity;

        int num = 50;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_SNAKE);
        }

		idxSnakeEnd = entity-1;
    }
    // Black holes
    {
		idxBlackHoleStart = entity;

        int num = 8;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_BLACKHOLE);
        }

		idxBlackHoleEnd = entity-1;
    }
    // Mayflies
    {
		idxMayflyStart = entity;

        int num = 200;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_MAYFLY);
        }

		idxMayflyEnd = entity-1;
    }
    // Protons
    {
		idxProtonStart = entity;

        int num = 200;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_PROTON);
        }

		idxProtonEnd = entity-1;
    }
    // Geoms
    {
		idxGeomSmallStart = entity;

        int num = 100;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_GEOM_SMALL);
        }

		idxGeomSmallEnd = entity-1;
    }
    {
		idxGeomMediumStart = entity;

        int num = 100;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_GEOM_MEDIUM);
        }

		idxGeomMediumEnd = entity-1;
    }
    {
		idxGeomLargeStart = entity;

        int num = 100;
        for (int i=0; i<num; i++)
        {
            mEnemies[entity++] = entity::createEntity(entity::ENTITY_TYPE_GEOM_LARGE);
        }

		idxGeomLargeEnd = entity-1;
    }

    TCHAR s[256];
    wsprintf(s, L"Num enemies created = %d\n", entity);
    OutputDebugString(s);

    // Fill the rest of the list with empty entries
    for (int i=entity; i<NUM_ENEMIES; i++)
    {
        mEnemies[i] = entity::createEntity(entity::ENTITY_TYPE_UNDEF);
    }

    for (int i=0; i<NUM_LINES; i++)
    {
        mLines[i] = static_cast<entityLine*>(entity::createEntity(entity::ENTITY_TYPE_LINE));
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
        if ((mEnemies[i]->getType() != entity::ENTITY_TYPE_GEOM_SMALL)
            && (mEnemies[i]->getType() != entity::ENTITY_TYPE_GEOM_MEDIUM)
            && (mEnemies[i]->getType() != entity::ENTITY_TYPE_GEOM_LARGE))
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
	int idxStart;
	int idxEnd;

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
		case entity::ENTITY_TYPE_MAYFLY:
			idxStart = idxMayflyStart;
			idxEnd = idxMayflyEnd;
			break;
		case entity::ENTITY_TYPE_PROTON:
			idxStart = idxProtonStart;
			idxEnd = idxProtonEnd;
			break;
		case entity::ENTITY_TYPE_GEOM_SMALL:
			idxStart = idxGeomSmallStart;
			idxEnd = idxGeomSmallEnd;
			break;
		case entity::ENTITY_TYPE_GEOM_MEDIUM:
			idxStart = idxGeomMediumStart;
			idxEnd = idxGeomMediumEnd;
			break;
		case entity::ENTITY_TYPE_GEOM_LARGE:
			idxStart = idxGeomLargeStart;
			idxEnd = idxGeomLargeEnd;
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
	int idxStart;
	int idxEnd;

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
		case entity::ENTITY_TYPE_MAYFLY:
			idxStart = idxMayflyStart;
			idxEnd = idxMayflyEnd;
			break;
		case entity::ENTITY_TYPE_PROTON:
			idxStart = idxProtonStart;
			idxEnd = idxProtonEnd;
			break;
		case entity::ENTITY_TYPE_GEOM_SMALL:
			idxStart = idxGeomSmallStart;
			idxEnd = idxGeomSmallEnd;
			break;
		case entity::ENTITY_TYPE_GEOM_MEDIUM:
			idxStart = idxGeomMediumStart;
			idxEnd = idxGeomMediumEnd;
			break;
		case entity::ENTITY_TYPE_GEOM_LARGE:
			idxStart = idxGeomLargeStart;
			idxEnd = idxGeomLargeEnd;
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
            pen.r *= 1.2;
            pen.g *= 1.2;
            pen.b *= 1.2;
            pen.a *= 1.2;
            if (pen.r > 1) pen.r = 1;
            if (pen.g > 1) pen.g = 1;
            if (pen.b > 1) pen.b = 1;
            if (pen.a > 1) pen.a = 1;
            pen.a = 1;
            line->setPen(pen);

            Point3d from = m->mVertexList[m->mEdgeList[i].from];
            Point3d to = m->mVertexList[m->mEdgeList[i].to];
            Point3d midpoint((from.x + to.x) / 2, (from.y + to.y) / 2, 0);

            line->addEndpoints(from, to);
            line->setPos(objectPos);
            line->setScale(e.getScale());
            line->setAngle(e.getAngle());

            line->setRotationRate((mathutils::frandFrom0To1() * .04) - .02);

            float angle = mathutils::calculate2dAngle(Point3d(0,0,0), midpoint);

            float variation = (mathutils::frandFrom0To1() * 2) - 1;

            Point3d speedVector(0,.2,0);
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
        mEnemies[i]->setState(entity::ENTITY_STATE_INACTIVE);
        mEnemies[i]->incGenId();
    }
}
