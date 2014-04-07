#include "scene.h"
#include "game.h"


#include "game.h"
game theGame;

int scene::mPass;
bool scene::mEnableGridGlow = true;

void glEnable2D();
void glDisable2D();

#define VIRTUAL_SCREEN_WIDTH 800
#define VIRTUAL_SCREEN_HEIGHT 600

static vector::pen singlePlayerColor(0, 1, 0, .6, 12);
static vector::pen player1Color(1, .6, .6, .75, 12);
static vector::pen player2Color(.6, .6, 1, .75, 12);


scene::scene()
{
	mAttractModeTimer = 0;
	mShowHighScores = FALSE;

    // Create the model for the shield symbol

    int i=0;

    mShieldSymbol.mNumVertex = 12;
    mShieldSymbol.mVertexList = new Point3d[mShieldSymbol.mNumVertex];
    mShieldSymbol.mVertexList[i++] = Point3d(0, 11.5);
    mShieldSymbol.mVertexList[i++] = Point3d(8.1, 8.1);
    mShieldSymbol.mVertexList[i++] = Point3d(11.5, 0);
    mShieldSymbol.mVertexList[i++] = Point3d(8.1, -8.1);
    mShieldSymbol.mVertexList[i++] = Point3d(0, -11.5);
    mShieldSymbol.mVertexList[i++] = Point3d(-8.1, -8.1);
    mShieldSymbol.mVertexList[i++] = Point3d(-11.5, 0);
    mShieldSymbol.mVertexList[i++] = Point3d(-8.1, 8.1);
    mShieldSymbol.mVertexList[i++] = Point3d(0, 4);
    mShieldSymbol.mVertexList[i++] = Point3d(0, -4);
    mShieldSymbol.mVertexList[i++] = Point3d(-4, 0);
    mShieldSymbol.mVertexList[i++] = Point3d(4, 0);

    i = 0;

    mShieldSymbol.mNumEdges = 10;
    mShieldSymbol.mEdgeList = new model::Edge[mShieldSymbol.mNumEdges];
    mShieldSymbol.mEdgeList[i].from = 0; mShieldSymbol.mEdgeList[i++].to = 1;
    mShieldSymbol.mEdgeList[i].from = 1; mShieldSymbol.mEdgeList[i++].to = 2;
    mShieldSymbol.mEdgeList[i].from = 2; mShieldSymbol.mEdgeList[i++].to = 3;
    mShieldSymbol.mEdgeList[i].from = 3; mShieldSymbol.mEdgeList[i++].to = 4;
    mShieldSymbol.mEdgeList[i].from = 4; mShieldSymbol.mEdgeList[i++].to = 5;
    mShieldSymbol.mEdgeList[i].from = 5; mShieldSymbol.mEdgeList[i++].to = 6;
    mShieldSymbol.mEdgeList[i].from = 6; mShieldSymbol.mEdgeList[i++].to = 7;
    mShieldSymbol.mEdgeList[i].from = 7; mShieldSymbol.mEdgeList[i++].to = 0;
    mShieldSymbol.mEdgeList[i].from = 8; mShieldSymbol.mEdgeList[i++].to = 9;
    mShieldSymbol.mEdgeList[i].from = 10; mShieldSymbol.mEdgeList[i++].to = 11;
}

void scene::run()
{
	theGame.run();
}

void scene::draw(int pass)
{
    mPass = pass;

	// *******************************************

    static float colorTimer=0;
    colorTimer += .08;

    vector::pen rainbowPen;
    rainbowPen.r = get_sin(colorTimer+((2*PI)/1))+.5;
    rainbowPen.g = get_sin(colorTimer+((2*PI)/2))+.5;
    rainbowPen.b = get_sin(colorTimer+((2*PI)/3))+.5;
    rainbowPen.a = 1;
    rainbowPen.lineRadius = 3;

	glClearColor(0,0,0,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPerspective(70, mWidth / mHeight, 0.01, 1000);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Glowy blending effect
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		theGame.draw(pass);

		glDisable(GL_BLEND);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	if ((game::mGameMode == game::GAMEMODE_HIGHSCORES_CHECK) || (game::mGameMode == game::GAMEMODE_HIGHSCORES))
	{
        float top = .8;
        float bottom = -.8;

        glEnable(GL_BLEND);

        glColor4f(0, 0, 0, .5);
	    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        glBegin( GL_QUADS );
        glVertex2d(-1.0, bottom);
        glVertex2d(1.0, bottom);
        glVertex2d(1.0, top);
        glVertex2d(-1.0, top);
        glEnd();


        glColor4f(0, 1, 0, .1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBegin( GL_QUADS );
        glVertex2d(-1.0, bottom);
        glVertex2d(1.0, bottom);
        glVertex2d(1.0, top);
        glVertex2d(-1.0, top);
        glEnd();

	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        if (pass == RENDERPASS_PRIMARY)
        {
            glLineWidth(6);

            glColor4f(0, 1, 0, .6);

            glBegin(GL_LINES);
            glVertex2d(1.0, top+.005);
            glVertex2d(-1.0, top+.005);
            glVertex2d(-1.0, bottom-.005);
            glVertex2d(1.0, bottom-.005);
            glEnd();
		}
	}
    else if ((game::mGameMode == game::GAMEMODE_ATTRACT)
        || (game::mGameMode == game::GAMEMODE_CREDITED)
        /*|| (game::mGameMode == game::GAMEMODE_HIGHSCORES)*/)
    {
        float top = .65;
        float bottom = -.4;

        glEnable(GL_BLEND);

        glColor4f(0, 0, 0, .5);
	    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        glBegin( GL_QUADS );
        glVertex2d(-1.0, bottom);
        glVertex2d(1.0, bottom);
        glVertex2d(1.0, top);
        glVertex2d(-1.0, top);
        glEnd();


        glColor4f(0, 1, 0, .1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

/*
        glBegin( GL_QUADS );
        glVertex2d(-1.0, bottom);
        glVertex2d(1.0, bottom);
        glVertex2d(1.0, top);
        glVertex2d(-1.0, top);
        glEnd();
*/

	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        if (pass == RENDERPASS_PRIMARY)
        {

            glLineWidth(6);

            glColor4f(.1, 1, .1, .4);
/*
            glBegin(GL_LINES);
            glVertex2d(1.0, top+.005);
            glVertex2d(-1.0, top+.005);
            glVertex2d(-1.0, bottom-.005);
            glVertex2d(1.0, bottom-.005);
            glEnd();
*/

            if (!mTextureMarquee.mLoaded)
                mTextureMarquee.load("images/marquee.png");

			// Draw the marquee
			if (!mShowHighScores)
			{
				// Glowy blending effect
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);

				// Enable the texture for OpenGL.
				glEnable( GL_TEXTURE_2D );
				glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

				mTextureMarquee.bind();

				glColor4f(1, 1, 1, 1);

				float x = 0;
				float y = 0;
				float width = 2;
				float height = 2/0.5625;

				glBegin(GL_QUADS);
				glTexCoord2d(0, 1); glVertex2d(x-(width/2), y-(height/2));
				glTexCoord2d(1, 1); glVertex2d(x+(width/2), y-(height/2));
				glTexCoord2d(1, 0); glVertex2d(x+(width/2), y+(height/2));
				glTexCoord2d(0, 0); glVertex2d(x-(width/2), y+(height/2));
				glEnd();

				mTextureMarquee.unbind();

				glDisable(GL_TEXTURE_2D);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
        }
    }

    if (pass == RENDERPASS_PRIMARY)
	{
		glEnable2D();

		// Glowy blending effect
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_MULTISAMPLE);

		if (game::mGameMode != game::GAMEMODE_HIGHSCORES)
			drawScores();

        // Game over mode
		if (game::mGameMode == game::GAMEMODE_HIGHSCORES)
		{
            drawCredits();
			game::mHighscore.drawEnterScore();
		}
        else if (game::mGameMode == game::GAMEMODE_GAMEOVER)
        {
/*
            drawCredits();

            vector::pen pen(1,1,1,.5,3);
            font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, -.2, &pen, "Game Over");

		    pen.a=.1;
		    pen.lineRadius = 8;
            font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, -.2, &pen, "Game Over");
*/
        }
        else if ((game::mGameMode == game::GAMEMODE_ATTRACT) || (game::mGameMode == game::GAMEMODE_CREDITED))
        {
			if (++mAttractModeTimer > (mShowHighScores ? 200 : 1000))
			{
				mAttractModeTimer = 0;
				mShowHighScores = !mShowHighScores;
			}

			if (mShowHighScores)
			{
				game::mHighscore.drawTable();
			}
			else
			{
				if (game::mGameMode == game::GAMEMODE_CREDITED)
				{
					drawCredits();

/*
                    vector::pen pen(1,1,1,.5,3);
                    font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, .0, &pen, "Game Over");

		            pen.a=.1;
		            pen.lineRadius = 8;
                    font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, .0, &pen, "Game Over");
*/

					static int flashTimer = 0;
					++flashTimer;

					if ((flashTimer / 30) & 1)
					{
						vector::pen pen(1,1,1,.5,3);
						if (game::mCredits == 1)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1 Player Start");
						else if (game::mCredits == 2)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1 or 2 Player Start");
						else if (game::mCredits == 3)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1, 2, or 3 Player Start");
						else if (game::mCredits == 4)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1, 2, 3, or 4 Player Start");

						pen.a=.1;
						pen.lineRadius = 8;
						if (game::mCredits == 1)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1 Player Start");
						else if (game::mCredits == 2)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1 or 2 Player Start");
						else if (game::mCredits == 3)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1, 2, or 3 Player Start");
						else if (game::mCredits == 4)
							font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Press 1, 2, 3, or 4 Player Start");
					}
				}
				else
				{
					drawCredits();

					vector::pen pen(1,1,1,.5,3);
					font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Insert Coins");

					pen.a=.1;
					pen.lineRadius = 8;
					font::AlphanumericsPrint(font::ALIGN_CENTER, .025, 0, -.2, &pen, "Insert Coins");
				}
			}
        }
        else // RUNNING
        {
            // Number of lives
            drawNumLives();
            drawNumBombs();
        }

        glDisable(GL_MULTISAMPLE);
        glDisable(GL_LINE_SMOOTH);

		glDisable2D();
    }

}

void scene::drawCredits()
{
    if (game::mFreeplay)
    {
/*
        vector::pen pen;
        pen.r=1;
        pen.g=1;
        pen.b=1;
        pen.a=.75;
        pen.lineRadius = 3;
        font::AlphanumericsPrint(font::ALIGN_CENTER, .01, 0, -.9, &pen, "FREE PLAY");

        pen.a=.2;
        pen.lineRadius = 8;
        font::AlphanumericsPrint(font::ALIGN_CENTER, .01, 0, -.9, &pen, "FREE PLAY");
*/
    }
    else
    {
        // Support for coin-op
        vector::pen pen;
        pen.r=1;
        pen.g=1;
        pen.b=1;
        pen.a=.75;
        pen.lineRadius = 3;
        font::AlphanumericsPrint(font::ALIGN_CENTER, .015, 0, -.9, &pen, "Credits %d", game::mCredits);

        pen.a=.2;
        pen.lineRadius = 8;
        font::AlphanumericsPrint(font::ALIGN_CENTER, .015, 0, -.9, &pen, "Credits %d", game::mCredits);
    }
}

void scene::drawNumLives()
{
    if (game::mNumPlayers == 2)
    {
        // 2 player display

        vector::pen pen(player1Color, .75, 12);
        float scale = .017;

        game::mPlayers.mPlayer1->getModel()->Identity();
        game::mPlayers.mPlayer1->getModel()->Scale(Point3d(scale, scale*mAspect, 0));
        game::mPlayers.mPlayer1->getModel()->Rotate(0);
        game::mPlayers.mPlayer1->getModel()->Translate(Point3d(-.9, .9, 0));
        game::mPlayers.mPlayer1->getModel()->draw(pen);

        pen = vector::pen(player1Color, .75, 3);
        font::AlphanumericsPrint(font::ALIGN_CENTER, .018, -.85, .9, &pen, "%d", game::m2PlayerNumLives);

        pen.a=.1;
        pen.lineRadius = 8;
        font::AlphanumericsPrint(font::ALIGN_CENTER, .018, -.85, .9, &pen, "%d", game::m2PlayerNumLives);
    }
    else
    {
        // 1 player display

        int overflow = 0;
        int num = game::mPlayers.mPlayer1->getNumLives();
        if (num > 5)
        {
            overflow = num;
            num = 1;
        }

        for (int i=0; i<num; i++)
        {
		    vector::pen pen(singlePlayerColor, .75, 12);
            float scale = .017;

            game::mPlayers.mPlayer1->getModel()->Identity();
            game::mPlayers.mPlayer1->getModel()->Scale(Point3d(scale, scale*mAspect, 0));
            game::mPlayers.mPlayer1->getModel()->Rotate(0);
            game::mPlayers.mPlayer1->getModel()->Translate(Point3d(-.12 + (.04*i), .9, 0));
            game::mPlayers.mPlayer1->getModel()->draw(pen);
        }

        if (overflow > 0)
        {
            vector::pen pen(singlePlayerColor, .75, 3);
            font::AlphanumericsPrint(font::ALIGN_CENTER, .016, -.12-.06, .9, &pen, "x%d", overflow);

	        pen.a=.1;
	        pen.lineRadius = 8;
            font::AlphanumericsPrint(font::ALIGN_CENTER, .016, -.12-.06, .9, &pen, "x%d", overflow);
        }
    }
}

void scene::drawNumBombs()
{
    if (game::mNumPlayers == 2)
    {
        // No bombs on 2 player game
    }
    else
    {
        // 1 player display

        int overflow = 0;
        int num = game::mPlayers.mPlayer1->getNumBombs();
        if (num > 5)
        {
            overflow = num;
            num = 1;
        }

        for (int i=0; i<num; i++)
        {
		    vector::pen pen(singlePlayerColor, .75, 12);
            float scale = .0014;

            mShieldSymbol.Identity();
            mShieldSymbol.Scale(Point3d(scale, scale*mAspect, 0));
            mShieldSymbol.Rotate(0);
            mShieldSymbol.Translate(Point3d(.12 + (.04*i), .9, 0));
            mShieldSymbol.draw(pen);
        }

        if (overflow > 0)
        {
            vector::pen pen(singlePlayerColor, .75, 3);
            font::AlphanumericsPrint(font::ALIGN_CENTER, .016, .12+.06, .9, &pen, "x%d", overflow);

	        pen.a=.1;
	        pen.lineRadius = 8;
            font::AlphanumericsPrint(font::ALIGN_CENTER, .016, .12+.06, .9, &pen, "x%d", overflow);
        }
    }
}

void scene::drawScores()
{
    BOOL gameover = (game::mGameMode != game::GAMEMODE_PLAYING);

    if (game::mNumPlayers == 2)
    {
        // 2 player display

        // Player 1 score display
        {
            char format[512];
            sprintf(format, "%d", game::mPlayers.mPlayer1->mScore);
            char* s = font::formatStringWithCommas(format);

		    vector::pen pen(player1Color, .75, 3);
            if (gameover)
		        font::AlphanumericsPrint(font::ALIGN_LEFT, .02, -.4, .9, &pen, "%s", s);
            else
    		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, -.4, .9, &pen, "%s x%d", s, game::mPlayers.mPlayer1->mMultiplier);

		    pen.a=.2;
		    pen.lineRadius = 8;
            if (gameover)
		        font::AlphanumericsPrint(font::ALIGN_LEFT, .02, -.4, .9, &pen, "%s", s);
            else
    		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, -.4, .9, &pen, "%s x%d", s, game::mPlayers.mPlayer1->mMultiplier);
        }

        // Player 2 score display
        if (game::mNumPlayers == 2)
        {
            char format[512];
            sprintf(format, "%d", game::mPlayers.mPlayer2->mScore);
            char* s = font::formatStringWithCommas(format);

		    vector::pen pen(player2Color, .75, 3);
            if (gameover)
		        font::AlphanumericsPrint(font::ALIGN_LEFT, .02, .4, .9, &pen, "%s", s);
            else
		        font::AlphanumericsPrint(font::ALIGN_LEFT, .02, .4, .9, &pen, "%s x%d", s, game::mPlayers.mPlayer1->mMultiplier);

		    pen.a=.2;
		    pen.lineRadius = 8;
            if (gameover)
		        font::AlphanumericsPrint(font::ALIGN_LEFT, .02, .4, .9, &pen, "%s", s);
            else
    		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, .4, .9, &pen, "%s x%d", s, game::mPlayers.mPlayer1->mMultiplier);
        }
    }
    else
    {
        // 1 player display

        // Player 1 score display
        {
            char format[512];
            sprintf(format, "%d", game::mPlayers.mPlayer1->mScore);
            char* s = font::formatStringWithCommas(format);

		    vector::pen pen(singlePlayerColor, .75, 3);
            if (gameover)
                font::AlphanumericsPrint(font::ALIGN_LEFT, .016, -.94, .9, &pen, "Score");
            else
                font::AlphanumericsPrint(font::ALIGN_LEFT, .016, -.94, .9, &pen, "Score x%d", game::mPlayers.mPlayer1->mMultiplier);
		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, -.94, .82, &pen, s);

		    pen.a=.2;
		    pen.lineRadius = 8;
            if (gameover)
                font::AlphanumericsPrint(font::ALIGN_LEFT, .016, -.94, .9, &pen, "Score");
            else
            font::AlphanumericsPrint(font::ALIGN_LEFT, .016, -.94, .9, &pen, "Score x%d", game::mPlayers.mPlayer1->mMultiplier);
		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, -.94, .82, &pen, s);
        }

/*
        // Player 2 score display
        if (game::mNumPlayers == 2)
        {
		    vector::pen pen(singlePlayerColor, .75, 3);
            font::AlphanumericsPrint(font::ALIGN_LEFT, .016, .69, .9, &pen, "Score x%d", game::mPlayers.mPlayer2->mMultiplier);
		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, .69, .82, &pen, "%d", game::mPlayers.mPlayer2->mScore);

		    pen.a=.2;
		    pen.lineRadius = 8;
            font::AlphanumericsPrint(font::ALIGN_LEFT, .016, .69, .9, &pen, "Score x%d", game::mPlayers.mPlayer2->mMultiplier);
		    font::AlphanumericsPrint(font::ALIGN_LEFT, .02, .69, .82, &pen, "%d", game::mPlayers.mPlayer2->mScore);
        }
*/
    }
}

void scene::glEnable2D()
{
	int vPort[4];
	vPort[0] = VIRTUAL_SCREEN_HEIGHT;
	vPort[1] = 0;
	vPort[2] = 0;
	vPort[3] = VIRTUAL_SCREEN_WIDTH;


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();


	glOrtho(vPort[0], vPort[0]+vPort[2], vPort[1]+vPort[3], vPort[1], -1, 1);

	glViewport(0, 0, mWidth, mHeight);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

}

void scene::glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void scene::showHighScores()
{
	if (game::mFreeplay || game::mCredits)
	{
		game::mGameMode = game::GAMEMODE_CREDITED;
	}
	else
	{
		game::mGameMode = game::GAMEMODE_ATTRACT;
	}
	mAttractModeTimer = -200;
	mShowHighScores = TRUE;
}

