#include "settings.h"

#include "game.h"


settings::settings()
{
    mCreditsPerGame = 0; // Freeplay

    mEnableGlow = true;

    mGridSmoothing = false;
    mParticleSmoothing = false;
    mEnemySmoothing = false;
    mPlayerSmoothing = true;
    mStarSmoothing = true;
}

settings::~settings()
{
}
