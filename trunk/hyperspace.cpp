#include "hyperspace.h"
#include "game.h"
#include "mathutils.h"

extern Point3d cameraPos;

const float size = 200;

hyperspace::hyperspace(void)
{
    for (int i=0; i<NUM_HS_STARS; i++)
    {
        mStars[i].lastPos.x = mStars[i].pos.x = (mathutils::frandFrom0To1() * size) - (size/2);
        mStars[i].lastPos.y = mStars[i].pos.y = (mathutils::frandFrom0To1() * size) - (size/2);
        mStars[i].lastPos.z = mStars[i].pos.z = (mathutils::frandFrom0To1() * size) - (size/2);
        mStars[i].color = vector::pen(mathutils::frandFrom0To1()+.5, mathutils::frandFrom0To1()+.5, mathutils::frandFrom0To1()+.5, 1, 1);
    }

    mCurrentBrightness = mTargetBrightness = 0;
}


hyperspace::~hyperspace(void)
{
}


void hyperspace::run()
{
    for (int i=0; i<NUM_HS_STARS; i++)
    {
        mStars[i].lastPos.z = mStars[i].pos.z;
        mStars[i].pos.z += 2 * mCurrentBrightness;
        if (mStars[i].pos.z > 100)
        {
            mStars[i].lastPos.x = mStars[i].pos.x = (mathutils::frandFrom0To1() * size) - (size/2);
            mStars[i].lastPos.y = mStars[i].pos.y = (mathutils::frandFrom0To1() * size) - (size/2);
            mStars[i].lastPos.z = mStars[i].pos.z = (mathutils::frandFrom0To1() * size) - (size/2);
            mStars[i].color = vector::pen(mathutils::frandFrom0To1()+.5, mathutils::frandFrom0To1()+.5, mathutils::frandFrom0To1()+.5, 1, 1);
        }
    }

    if (mCurrentBrightness < mTargetBrightness)
    {
        mCurrentBrightness += .005;
    }
    else if (mCurrentBrightness > mTargetBrightness)
    {
        mCurrentBrightness *= .99;
    }
}

void hyperspace::draw()
{

    glEnable(GL_POINT_SMOOTH);
    glPointSize(4);
    glColor4f(1, 1, 1, 1);

    glBegin(GL_LINES);

    for (int i=0; i<NUM_HS_STARS; i++)
    {
//        glColor4f(mStars[i].color.r, mStars[i].color.g, mStars[i].color.b, mCurrentBrightness * 1);
        glColor4f(1,1,1,.5 * mCurrentBrightness);

        glVertex3d(mStars[i].lastPos.x + game::mCamera.mCurrentPos.x, mStars[i].lastPos.y + game::mCamera.mCurrentPos.y, mStars[i].lastPos.z + game::mCamera.mCurrentPos.z);
        glVertex3d(mStars[i].pos.x + game::mCamera.mCurrentPos.x, mStars[i].pos.y + game::mCamera.mCurrentPos.y, mStars[i].pos.z + game::mCamera.mCurrentPos.z);
    }

    glEnd();
}

