#ifndef _SCENE_H
#define _SCENE_H

#include "model.h"
#include "texture.h"


class scene
{
public:

    enum RenderPass
    {
        RENDERPASS_PRIMARY=0,
        RENDERPASS_BLUR
    };

    scene();

    void run();
    void draw(int pass);
	void size(float w, float h) { mWidth = w; mHeight = h; mAspect = w/h; }

	void glEnable2D();
	void glDisable2D();

	void showHighScores();

    static int mPass;
    static bool mEnableGridGlow;


private:
	float mWidth;
	float mHeight;
    float mAspect;

    void drawCredits();
    void drawNumBombs();
    void drawNumLives();
    void drawScores();

	int mAttractModeTimer;
	bool mShowHighScores;

    model mShieldSymbol;

    texture mTextureMarquee;
};

#endif
