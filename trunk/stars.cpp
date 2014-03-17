#include "stars.h"
#include "game.h"



stars::stars()
{
    const float overscan = 700;
    const float leftEdge = 0-overscan;
    const float bottomEdge = 0-overscan;
    const float rightEdge = (theGame.mGrid.extentX() + overscan);
    const float topEdge = (theGame.mGrid.extentY() + overscan);

    mStars = new STAR[NUM_STARS];

    for (int i=0; i<NUM_STARS; i++)
    {
        float z;
        if (i < 2000)
        {
            // Layer 1
            z = -20;
        }
        else if (i < 4000)
        {
            // Layer 2
            z = -40;
        }
        else if (i < 6000)
        {
            // Layer 3
            z = -60;
        }
        else // if (i < 8000)
        {
            // Layer 4
            z = -80;
        }

        mStars[i].pos.x = mathutils::randFromTo(leftEdge, rightEdge);
        mStars[i].pos.y = mathutils::randFromTo(bottomEdge, topEdge);
        mStars[i].pos.z = z;
        mStars[i].radius = ((mathutils::frandFrom0To1()*.5) + .25);

    }
}

stars::~stars()
{
    delete mStars;
}

void stars::run()
{
}

void stars::draw()
{
    float brightness = 1;//game::mGrid.brightness;

    if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED)
        brightness = 1;

    if (brightness <= 0)
        return;

//    glEnable(GL_POINT_SMOOTH);
    glPointSize(4);

    glBegin(GL_POINTS);

    for (int i=0; i<NUM_STARS; i++)
    {
        Point3d pt = mStars[i].pos;

        glColor4f(1.0f, 1.0f, 1.0f, (mStars[i].radius+.5) * brightness);
        glVertex3d(pt.x, pt.y, pt.z);
    }

    glEnd();
    glDisable(GL_POINT_SMOOTH);
}

