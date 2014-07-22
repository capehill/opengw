#include "entityMayfly.h"
#include "game.h"

entityMayfly::entityMayfly()
    : entity()
{
    mScale = 0.8;
    mRadius = 2.5;

    mScoreValue = 50;

    mType = ENTITY_TYPE_MAYFLY;
    setState(ENTITY_STATE_INACTIVE);

    mFlipTimer = mathutils::frandFrom0To1() * 15;
    mFlipDirection = 1;

    mPen = vector::pen(.5, .5, 1, 1, 12);

    int i = 0;

    mModel.mNumVertex = 6;
    mModel.mVertexList = new Point3d[mModel.mNumVertex];

    mModel.mVertexList[i++] = Point3d(-.25, 1.25);
    mModel.mVertexList[i++] = Point3d(.25, 1.25);

    mModel.mVertexList[i++] = Point3d(1.2, -.5);
    mModel.mVertexList[i++] = Point3d(1, -.9);

    mModel.mVertexList[i++] = Point3d(-1, -.9);
    mModel.mVertexList[i++] = Point3d(-1.2, -.5);

    i = 0;

    mModel.mNumEdges = 3;
    mModel.mEdgeList = new model::Edge[mModel.mNumEdges];
    mModel.mEdgeList[i].from = 0; mModel.mEdgeList[i++].to = 3;
    mModel.mEdgeList[i].from = 1; mModel.mEdgeList[i++].to = 4;
    mModel.mEdgeList[i].from = 2; mModel.mEdgeList[i++].to = 5;
}

void entityMayfly::draw()
{
    if (this->getState() == entity::ENTITY_STATE_INDICATING)
    {
        if (((int)(mStateTimer/10)) & 1)
        {
            vector::pen pen = mPen;
            mModel.draw(pen);
        }
    }
    else if (this->getEnabled() && (this->getState() != entity::ENTITY_STATE_SPAWN_TRANSITION))
    {
        vector::pen pen = mPen;
        if (scene::mPass == scene::RENDERPASS_BLUR)
        {
            pen.r = .1;
            pen.g = .1;
            pen.b = 1;
            pen.a = 1;
            pen.lineRadius = 18*2;
        }

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

            progress = progress + .25;

            a = 1-progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            pen.a = a;

            mModel.Identity();
            mModel.Scale(scale * progress * 4);
            mModel.Rotate(mAngle);
            mModel.Translate(trans);
            mModel.emit(pen);

            // *********************************************

            progress = progress + .25;

            a = 1-progress;
            if (a<0) a = 0;
            if (a>1) a = 1;

            pen.a = a;

            mModel.Identity();
            mModel.Scale(scale * progress * 7);
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
}

void entityMayfly::run()
{
    if (this->getEnabled())
    {
        // Seek the player

        // Run animation
        if (--mFlipTimer <= 0)
        {
            mFlipTimer = 15;
            mFlipDirection = -mFlipDirection;

            // Update the target
            mTarget = game::mPlayers.getPlayerClosestToPosition(mPos)->getPos();
            mTarget.x += (mathutils::frandFrom0To1() * 30) - 15;
            mTarget.y += (mathutils::frandFrom0To1() * 30) - 15;
        }

        float desiredAngle = 1.2 * mFlipDirection;
        float diff = desiredAngle-mAngle;
        mRotationRate += diff * .03;
        mRotationRate *= .9;

        float angle = mathutils::calculate2dAngle(mPos, mTarget);
        Point3d moveVector(1, 0, 0);
        moveVector = mathutils::rotate2dPoint(moveVector, angle);
        mSpeed += moveVector * .5;
        mSpeed = mathutils::clamp2dVector(mSpeed, .45 * mAggression);

        mSpeed *= .9;
    }
    entity::run();
}

