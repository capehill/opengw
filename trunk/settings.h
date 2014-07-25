#ifndef __SETTINGS_H__
#define __SETTINGS_H__


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

};


#endif // __SETTINGS_H__