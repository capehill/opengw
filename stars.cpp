#include "game.hpp"
#include "grid.hpp"
#include "mathutils.hpp"
#include "stars.hpp"

#include "SDL_opengl.h"

stars::stars(const game& gameRef) : mGame(gameRef)
{
    const float overscan = 700;
    const float leftEdge = 0 - overscan;
    const float bottomEdge = 0 - overscan;
    const float rightEdge = (mGame.mGrid->extentX() + overscan);
    const float topEdge = (mGame.mGrid->extentY() + overscan);

    constexpr int NUM_STARS = 8000;

    mStars.resize(NUM_STARS);

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
}

void stars::run()
{
    for (auto& star : mStars) {
        if (mathutils::frandFrom0To1() > 0.98f) {
            star.twinkle = .5f;
        } else {
            star.twinkle = 1.0f;
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

    for (const auto& star : mStars) {
        glColor4f(1.0f, 1.0f, 1.0f, (star.radius + 0.5f) * brightness * star.brightness * star.twinkle);
        glVertex3d(star.pos.x, star.pos.y, star.pos.z);
    }

    glEnd();
}
