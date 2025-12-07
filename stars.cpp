#include "stars.hpp"
#include "game.hpp"

#include "SDL_opengl.h"

stars::stars(const game& gameRef) : mGame(gameRef)
{
    const float overscan = 700;
    const float leftEdge = 0 - overscan;
    const float bottomEdge = 0 - overscan;
    const float rightEdge = (mGame.mGrid->extentX() + overscan);
    const float topEdge = (mGame.mGrid->extentY() + overscan);

    mStars = new STAR[NUM_STARS];

    for (int i = 0; i < NUM_STARS; i++) {
        float z;
        if (i < 2000) {
            // Layer 1
            z = -20;
        } else if (i < 4000) {
            // Layer 2
            z = -40;
        } else if (i < 6000) {
            // Layer 3
            z = -60;
        } else // if (i < 8000)
        {
            // Layer 4
            z = -80;
        }

        mStars[i].pos.x = mathutils::randFromTo(leftEdge, rightEdge);
        mStars[i].pos.y = mathutils::randFromTo(bottomEdge, topEdge);
        mStars[i].pos.z = z;
        mStars[i].radius = ((mathutils::frandFrom0To1() * 0.5f) + 0.25f);
        mStars[i].brightness = ((mathutils::frandFrom0To1() * 0.75f) + 0.25f);
        mStars[i].twinkle = 1.0f;
    }
}

stars::~stars()
{
    delete[] mStars;
}

void stars::run()
{
    for (int i = 0; i < NUM_STARS; i++) {
        if (mathutils::frandFrom0To1() > 0.98f) {
            mStars[i].twinkle = .5f;
        } else {
            mStars[i].twinkle = 1.0f;
        }
    }
}

void stars::draw()
{
    constexpr float brightness = 1.0f; // game::mGrid->brightness;

    /*
        if (mGame.mGameMode == game::GAMEMODE_ATTRACT || mGame.mGameMode == game::GAMEMODE_CREDITED)
            brightness = 1.0f;

        if (brightness <= 0.0f)
            return;
    */
    glPointSize(4.0f);

    glBegin(GL_POINTS);

    for (int i = 0; i < NUM_STARS; i++) {
        glColor4f(1.0f, 1.0f, 1.0f, (mStars[i].radius + 0.5f) * brightness * mStars[i].brightness * mStars[i].twinkle);
        glVertex3d(mStars[i].pos.x, mStars[i].pos.y, mStars[i].pos.z);
    }

    glEnd();
}
