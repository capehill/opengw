#pragma once

class settings
{
  public:
    settings();
    ~settings() = default;

    const static settings& get();

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

    int displayWidth;
    int displayHeight;
};
