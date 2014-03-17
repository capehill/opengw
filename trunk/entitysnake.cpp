#include "entitysnake.h"
#include "game.h"

#define NUM_SEGMENTS 23
#define NUM_SEG_STREAM_ITEMS 5 // sets the spacing between segments


// PERFORMANCE: ~Something~ about the snakes causes a huge slowdown. No idea if it's the drawing or something else in the run() code...

class entitySnakeSegment : public entity
{
public:

    typedef struct
    {
        Point3d pos;
        float angle;
    }SegmentStreamItem;

    float mTail;
    entity* mParent;

    entitySnakeSegment()
    {
        mScale = 1; // gets sized dynamically by the head
        mRadius = 1;

        mScoreValue = 0;

        mTail = 0;

        mDestroyTime = 0;

        mType = ENTITY_TYPE_SNAKE_SEGMENT;

        mPen = vector::pen(1, 1, .8, .5, 12);

        int i = 0;

        mModel.mNumVertex = 3;
        mModel.mVertexList = new Point3d[mModel.mNumVertex];
        mModel.mVertexList[i++] = Point3d(0, .8);
        mModel.mVertexList[i++] = Point3d(.5, -.5);
        mModel.mVertexList[i++] = Point3d(-.5, -.5);

        i = 0;

        mModel.mNumEdges = 3;
        mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
		mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
		mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
		mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 0;

        mSegmentStream = new SegmentStreamItem[NUM_SEG_STREAM_ITEMS];
        for (int i=0; i<NUM_SEG_STREAM_ITEMS; i++)
        {
            mSegmentStream[i].pos = mPos;
            mSegmentStream[i].angle = mAngle;
        }
    }

    void setParent(entity* parent)
    {
        mParent = parent;
    }

    virtual entity* getParent()
    {
        return mParent;
    }

    void setStreamHead(const SegmentStreamItem& item)
    {
        mSegmentStream[0].pos = item.pos;
        mSegmentStream[0].angle = item.angle;
    }

    const SegmentStreamItem getStreamHead()
    {
        return mSegmentStream[0];
    }

    const SegmentStreamItem getStreamTail()
    {
        return mSegmentStream[(int)mTail];
    }

    void draw()
    {
        if (this->getState() == entity::ENTITY_STATE_INDICATING)
        {
            if (((int)(mParent->getStateTimer()/10)) & 1)
            {
                vector::pen pen = mPen;
                if (scene::mPass == scene::RENDERPASS_BLUR)
                {
                    pen.r = 1;
                    pen.g = .5;
                    pen.b = .1;
                    pen.lineRadius = 18;
                }
                mModel.draw(pen);
            }
        }
        else if (getEnabled())
        {
            vector::pen pen = mPen;
            if (scene::mPass == scene::RENDERPASS_BLUR)
            {
                pen.r = 1;
                pen.g = .5;
                pen.b = .1;
                pen.lineRadius = 18;
            }
            mModel.draw(pen);
        }
    }

    void drawSpawn(int index, int total)
    {
        vector::pen pen = mPen;

        Point3d scale = mScale;
        Point3d trans = mPos;

        float inc = 1.0f / total;
        float progress = index * inc;

        // *********************************************
        {

            progress = 1-progress;

            float a = progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            pen.a = a;

            mModel.Identity();
            Point3d s(scale.x * progress * 1, scale.y, scale.z);
            mModel.Scale(s);
            mModel.Rotate(mAngle);
            mModel.Translate(trans);
            mModel.draw(pen);

        }

        // Restore stuff
//        mModel.Identity();
//        mModel.Rotate(mAngle);
//        mModel.Scale(scale);
//        mModel.Translate(trans);

        // Draw it normally too
//        mModel.draw(pen);
    }

    void run()
    {
        entity::run();

        setPos(mSegmentStream[0].pos);
        setAngle(mSegmentStream[0].angle);

        // Shift the stream from head to tail
        for (int i=NUM_SEG_STREAM_ITEMS-2; i>=0; i--)
        {
            mSegmentStream[i+1].pos = mSegmentStream[i].pos;
            mSegmentStream[i+1].angle = mSegmentStream[i].angle;
        }

        if (mTail < NUM_SEG_STREAM_ITEMS-1)
        {
            mTail += .1;
            if (mTail > NUM_SEG_STREAM_ITEMS-1)
                mTail = NUM_SEG_STREAM_ITEMS-1;
        }
    }

    void spawnTransition()
    {
        entity::spawnTransition();
		mTail = NUM_SEG_STREAM_ITEMS-1;
    }

	void postSpawnTransition()
	{
        // Set up all the segments
        for (int i=0; i<NUM_SEG_STREAM_ITEMS; i++)
        {
            mSegmentStream[i].pos = mPos;
            mSegmentStream[i].angle = mAngle;
        }
	}

    void destroyTransition()
    {
        setState(ENTITY_STATE_DESTROYED);
        mStateTimer = mDestroyTime;

        // Throw out some particles
        Point3d pos(this->mPos);
        Point3d angle(0,0,0);
        float speed = 1.2;
        float spread = 2*PI;
        int num = 10;
        int timeToLive = 150;
        vector::pen pen = mPen;
        pen.r *= 1.2;
        pen.g *= 1.2;
        pen.b *= 1.2;
        pen.a = .8;
        pen.lineRadius=5;
        game::mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, TRUE, TRUE, .98, TRUE);

        // Explode the object into line entities
        game::mEnemies.explodeEntity(*this);
    }

    void destroy()
    {
        entity::destroy();
    }

    void hit(entity* aEntity)
    {
        // Do nothing and don't call the base class method (tail segments are invinsible)
    }

    SegmentStreamItem* mSegmentStream;

};

//////////////////////////////////////////////////////////////////////////

entitySnake::entitySnake()
{
    mScale = 1.25;
    mRadius = 3;

    mScoreValue = 50;

    mType = ENTITY_TYPE_SNAKE;
    setState(ENTITY_STATE_INACTIVE);

    mPen = vector::pen(.5, .5, 1, 2, 12);
    mModel.mIsLineLoop = TRUE;

    int i=0;

    mModel.mNumVertex = 14;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];
    mModel.mVertexList[i++] = Point3d(0, 1.53);
    mModel.mVertexList[i++] = Point3d(.4, 1.404);
    mModel.mVertexList[i++] = Point3d(.71, 1.0575);
    mModel.mVertexList[i++] = Point3d(.9, 0.558);
    mModel.mVertexList[i++] = Point3d(.9, 0.135);
    mModel.mVertexList[i++] = Point3d(.68, -0.288);
    mModel.mVertexList[i++] = Point3d(.46, -0.513);
    mModel.mVertexList[i++] = Point3d(0, 0);
    mModel.mVertexList[i++] = Point3d(-.46, -0.513);
    mModel.mVertexList[i++] = Point3d(-.68, -0.288);
    mModel.mVertexList[i++] = Point3d(-.9, 0.135);
    mModel.mVertexList[i++] = Point3d(-.9, 0.558);
    mModel.mVertexList[i++] = Point3d(-.71, 1.0575);
    mModel.mVertexList[i++] = Point3d(-.4, 1.404);

/*
    // FIX THIS
    for (i=0; i<mModel.mNumVertex; i++)
    {
        mModel.mVertexList[i].y -= 1.4;
    }
*/
    i = 0;

    mModel.mNumEdges = 15;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];

    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 1;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 2;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 3; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 4; mModel.mEdgeList[i++].to = 5;
    mModel.mEdgeList[i].from = 5; mModel.mEdgeList[i++].to = 6;
    mModel.mEdgeList[i].from = 6; mModel.mEdgeList[i++].to = 7;
    mModel.mEdgeList[i].from = 7; mModel.mEdgeList[i++].to = 8;
    mModel.mEdgeList[i].from = 8; mModel.mEdgeList[i++].to = 9;
    mModel.mEdgeList[i].from = 9; mModel.mEdgeList[i++].to = 10;
    mModel.mEdgeList[i].from = 10; mModel.mEdgeList[i++].to = 11;
    mModel.mEdgeList[i].from = 11; mModel.mEdgeList[i++].to = 12;
    mModel.mEdgeList[i].from = 12; mModel.mEdgeList[i++].to = 13;
    mModel.mEdgeList[i].from = 13; mModel.mEdgeList[i++].to = 0;

    // Create the tail segments
    mSegments = new entitySnakeSegment[NUM_SEGMENTS];
    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        mSegments[i].setState(ENTITY_STATE_INACTIVE);

        entitySnakeSegment::SegmentStreamItem item;
        item.pos = mPos;
        item.angle = mAngle;
        mSegments[i].setStreamHead(item);
        Point3d scale((1-((float)i/NUM_SEGMENTS)) * 2, 1.4, 1);
        if (scale.x > 1.1) scale.x = 1.1;
        mSegments[i].setScale(scale);
        mSegments[i].setParent(this);
    }
}

void entitySnake::runTransition()
{
    entity::runTransition();
}

void entitySnake::run()
{
    if (this->getEnabled())
    {
        float targetDistance = mathutils::calculate2dDistance(mPos, mTarget);
        if (targetDistance < 10)
        {
            // Pick a new target
            updateTarget();
        }

        float diff = (mathutils::calculate2dAngle(mPos, mTarget) - mathutils::DegreesToRads(90));

        if (fabs(diff) > .1)
        {
            if (mAngle < diff)
            {
                mRotationRate += .005;
            }
            else if ((mAngle > diff))
            {
                mRotationRate -= .005;
            }
        }

        mRotationRate *= .98;

        const float maxRate = 2;

        if (mRotationRate > maxRate)
            mRotationRate = maxRate;
        else if (mRotationRate < -maxRate)
            mRotationRate = -maxRate;


        Point3d moveVector(0, 1, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, mAngle);
        mSpeed += moveVector;
        mSpeed = mathutils::clamp2dVector(mSpeed, .5);

        mSpeed *= .95;

        // Segments

        // Set the first segment head
        {
            entitySnakeSegment::SegmentStreamItem item;
            item.pos = mPos;
            item.angle = mAngle;
            mSegments[0].setStreamHead(item);
        }

        // Link them all together
        for (int i=NUM_SEGMENTS-2; i>=0; i--)
        {
            entitySnakeSegment* segmentThis = &mSegments[i];
            entitySnakeSegment* segmentNext = &mSegments[i+1];
            segmentNext->setStreamHead(segmentThis->getStreamTail());
        }

        // Run each
        for (int i=0; i<NUM_SEGMENTS; i++)
        {
            mSegments[i].run();
        }

    }
    entity::run();


}

void entitySnake::spawnTransition()
{
    entity::spawnTransition();

	// Aim towards the closest player
	mAngle = mathutils::calculate2dAngle(mPos, game::mPlayers.getPlayerClosestToPosition(mPos)->getPos()) + mathutils::DegreesToRads(-90);

    updateTarget();

	// Position and aim the tail segments correctly for a spawn
    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        mSegments[i].spawnTransition();

		Point3d posVector(0, (i+1) * -.2, 0);
		posVector = mathutils::rotate2dPoint(posVector, mAngle);
        mSegments[i].setPos(mPos + posVector);
        mSegments[i].setAngle(mAngle);

		mSegments[i].postSpawnTransition();
    }

	game::mSound.playTrack(SOUNDID_ENEMYSPAWN6A);
}

void entitySnake::spawn()
{
    entity::spawn();

    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        mSegments[i].spawn();
    }
}

void entitySnake::destroyTransition()
{
    entity::destroyTransition();

    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        mSegments[i].destroyTransition();
    }
}

void entitySnake::destroy()
{
    entity::destroy();

    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        mSegments[i].destroy();
    }
}

entity* entitySnake::hitTest(const Point3d& pos, float radius)
{
    // First see if it hit the head
    entity* e = entity::hitTest(pos, radius);
    if (e) return e;

    // Nope, check the tail segments
    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        entitySnakeSegment* segment = &mSegments[i];
        Point3d ourPos(0,0,0);
        segment->getModel()->mMatrix.TransformVertex(ourPos, &ourPos);

        float distance = mathutils::calculate2dDistance(pos, ourPos);
        float resultRadius = radius + segment->getRadius();
        if (distance < resultRadius)
        {
            return segment;
        }
    }

    return NULL;
}

void entitySnake::draw()
{
//    entity::draw();


    if (this->getState() == entity::ENTITY_STATE_INDICATING)
    {
        if (((int)(mStateTimer/10)) & 1)
        {
            vector::pen pen = mPen;
            mModel.draw(pen);
        }
    }
    else if (this->getEnabled())
    {
        vector::pen pen = mPen;

        if (getState() == ENTITY_STATE_SPAWNING)
        {
            Point3d scale = mScale;
            Point3d trans = mPos;

            float inc = 1.0f / mSpawnTime;
            float progress = mStateTimer * inc;

            // *********************************************

		    glLineWidth(pen.lineRadius*.3);
			glBegin(GL_LINES);

            progress = 1-progress;
			
            float a = progress;
            if (a<0) a = 0;
            if (a>1) a = 1;
			
            pen.a = a;

            mModel.Identity();
            mModel.Scale(scale * progress * 1);
            mModel.Rotate(mAngle);
            mModel.Translate(trans);
            mModel.emit(pen);
			
            // *********************************************

            // Restore stuff
            mModel.Identity();
            mModel.Rotate(mAngle);
            mModel.Scale(scale);
            mModel.Translate(trans);

			glEnd();
        }

        mModel.draw(pen);
    }



    if (getState() == ENTITY_STATE_SPAWNING)
    {
        for (int i=0; i<NUM_SEGMENTS; i++)
        {
            mSegments[i].drawSpawn(mStateTimer, mSpawnTime);
        }
    }
    else
    {
        for (int i=0; i<NUM_SEGMENTS; i++)
        {
            mSegments[i].draw();
        }
    }
    
}

void entitySnake::indicateTransition()
{
    entity::indicateTransition();
    for (int i=0; i<NUM_SEGMENTS; i++)
    {
        mSegments[i].setState(ENTITY_STATE_INDICATING);
    }
}


void entitySnake::updateTarget()
{
    // Pick a random point around us
    const float distance = 40;

    float angle = mathutils::frandFrom0To1() * (2*PI);
    mTarget = Point3d(distance, 0, 0);
    mTarget = mathutils::rotate2dPoint(mTarget, angle);
    mTarget += mPos;

    const float margin = 15;
    const float leftEdge = margin;
    const float bottomEdge = margin;
    const float rightEdge = (theGame.mGrid.extentX()-1)-margin;
    const float topEdge = (theGame.mGrid.extentY()-1)-margin;

    if (mTarget.x < leftEdge)
        mTarget.x = leftEdge;
    else if (mTarget.x > rightEdge)
        mTarget.x = rightEdge;
    if (mTarget.y < bottomEdge)
        mTarget.y = bottomEdge;
    else if (mTarget.y > topEdge)
        mTarget.y = topEdge;
}








