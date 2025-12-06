#pragma once

class settings
{
public:

    settings();
    ~settings();

    bool mCreditsPerGame;

    bool mEnableGlow;

    bool mGridSmoothing;
    bool mParticleSmoothing;
    bool mEnemySmoothing;
    bool mPlayerSmoothing;
    bool mStarSmoothing;

    int mAttractors;
    int mParticles;
    int mGridPasses;
};
