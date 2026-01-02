#include "settings.hpp"

#include <memory>

static std::unique_ptr<settings> instance;

const settings& settings::get()
{
    if (!instance) {
        instance = std::make_unique<settings>();
    }

    return *instance;
}

settings::settings()
{
    mCreditsPerGame = false; // false = Freeplay

    mEnableGlow = true;

    mGridSmoothing = false;
    mParticleSmoothing = false;
    mEnemySmoothing = false;
    mPlayerSmoothing = true;
    mStarSmoothing = true;

    mAttractors = 50;
    mParticles = 3000;

    mGridPasses = 4;

    displayWidth = 800;
    displayHeight = 600;
}
