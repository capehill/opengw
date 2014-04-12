#include "highscore.h"
#include "game.h"

extern scene oglScene;
extern vector::pen defaultFontPen;

static char charList[] =
{
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	' ',
	NULL,
};


highscore::highscore(void)
{
	for (int i=0; i<10; i++)
	{
		strcpy(mEntries[i].name, "   ");
		mEntries[i].score = 0;
	}

	mLastLetter = 0;
	for (int i=0; charList[i] != NULL; i++)
	{
		++mLastLetter;
	}

	--mLastLetter;

	mEditCurrentLetter[0] = 0;
	mEditCurrentLetter[1] = 0;
	mEditCurrentLetter[2] = 0;

    load();
}

highscore::~highscore(void)
{
    save();
}

bool highscore::isHighScore(int score)
{
	for (int i=0; i<10; i++)
	{
		if (score > mEntries[i].score)
		{
			return true;
		}
	}
	return false;
}

void highscore::init()
{
	mScore = game::mPlayers.mPlayer1->mScore;
	mEditCurrentPos = 0;
	strcpy(mEditName, "   ");
}

void highscore::run()
{
	if (--mDebounceTimer <= 0)
	{
		mDebounceTimer = 20;

        bool trigger = game::mControls.getTriggerButton(0);
		if (trigger)
		{
			// All done
            save();
			mEditName[0] = charList[mEditCurrentLetter[0]];
			mEditName[1] = charList[mEditCurrentLetter[1]];
			mEditName[2] = charList[mEditCurrentLetter[2]];
			mEditName[3] = '\0';
			addHighScore(mEditName, mScore);
			oglScene.showHighScores();
		}

		Point3d leftStick = game::mControls.getLeftStick(0);
		float distance = mathutils::calculate2dDistance(Point3d(0,0,0), leftStick);
		if (distance > .1)
		{
			float angle = mathutils::RadsToDegrees(mathutils::calculate2dAngle(Point3d(0,0,0), leftStick));
			if ((angle > 90) && (angle < 270))
			{
				// Down
				++mEditCurrentLetter[mEditCurrentPos];
				if (mEditCurrentLetter[mEditCurrentPos] > mLastLetter)
				{
					mEditCurrentLetter[mEditCurrentPos] = 0;
				}
			}
			else
			{
				// Up
				--mEditCurrentLetter[mEditCurrentPos];
				if (mEditCurrentLetter[mEditCurrentPos] <= 0)
				{
					mEditCurrentLetter[mEditCurrentPos] = mLastLetter;
				}
			}

			mEditName[mEditCurrentPos] = charList[mEditCurrentLetter[mEditCurrentPos]];
		}

		Point3d rightStick = game::mControls.getRightStick(0);
		distance = mathutils::calculate2dDistance(Point3d(0,0,0), rightStick);
		if (distance > .1)
		{
			float angle = mathutils::RadsToDegrees(mathutils::calculate2dAngle(Point3d(0,0,0), rightStick));
			if ((angle > 90) && (angle < 270))
			{
				// Right
				++mEditCurrentPos;
				if (mEditCurrentPos >= 3)
				{
					mEditCurrentPos = 0;
				}
			}
			else
			{
				// Left
				--mEditCurrentPos;
				if (mEditCurrentPos < 0)
				{
					mEditCurrentPos = 2;
				}
			}
		}

	}

}

void highscore::addHighScore(char* name, int score)
{
	for (int i=0; i<10; i++)
	{
		if (score >= mEntries[i].score)
		{
			// Move all the previous entries down and drop the last entry off the list
			for (int j=10-2; j>=i; j--)
			{
				strcpy(mEntries[j+1].name, mEntries[j].name);
				mEntries[j+1].score = mEntries[j].score;
			}

			// Plop the new score in-place
			strcpy(mEntries[i].name, name);
			mEntries[i].score = score;
			break;
		}
	}
}

void highscore::drawEnterScore()
{
    vector::pen pen = defaultFontPen;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, .6, &pen, "High Score!");
	pen.a=.1;
	pen.lineRadius = 8;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, .6, &pen, "High Score!");

	{
		pen = defaultFontPen;

		char format[512];
		sprintf(format, "%d", mScore);
		char* s = font::formatStringWithCommas(format);

		font::AlphanumericsPrint(font::ALIGN_CENTER, .048, 0, .3, &pen, s);
		pen.a=.1;
		pen.lineRadius = 8;
		font::AlphanumericsPrint(font::ALIGN_CENTER, .048, 0, .3, &pen, s);
	}

    pen = defaultFontPen;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .02, 0, .1, &pen, "Please Enter Your Initials");
	pen.a=.1;
	pen.lineRadius = 8;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .02, 0, .1, &pen, "Please Enter Your Initials");

	// Name edit area
	{
		if (mEditCurrentPos == 0)
		{
			pen = vector::pen(1,1,1,1,3);
			font::AlphanumericsPrint(font::ALIGN_CENTER, .06, -.1, -.2, &pen, "%c", charList[mEditCurrentLetter[0]]);
		}
		else
		{
			pen = vector::pen(1,1,1,.5,3);
			font::AlphanumericsPrint(font::ALIGN_CENTER, .06, -.1, -.2, &pen, "%c", charList[mEditCurrentLetter[0]]);
		}

		if (mEditCurrentPos == 1)
		{
			pen = vector::pen(1,1,1,1,3);
			font::AlphanumericsPrint(font::ALIGN_CENTER, .06, 0, -.2, &pen, "%c", charList[mEditCurrentLetter[1]]);
		}
		else
		{
			pen = vector::pen(1,1,1,.5,3);
			font::AlphanumericsPrint(font::ALIGN_CENTER, .06, 0, -.2, &pen, "%c", charList[mEditCurrentLetter[1]]);
		}

		if (mEditCurrentPos == 2)
		{
			pen = vector::pen(1,1,1,1,3);
			font::AlphanumericsPrint(font::ALIGN_CENTER, .06, .1, -.2, &pen, "%c", charList[mEditCurrentLetter[2]]);
		}
		else
		{
			pen = vector::pen(1,1,1,.5,3);
			font::AlphanumericsPrint(font::ALIGN_CENTER, .06, .1, -.2, &pen, "%c", charList[mEditCurrentLetter[2]]);
		}

		// Underlines
		pen = (mEditCurrentPos == 0) ? vector::pen(1,1,1,1,3) : vector::pen(1,1,1,.2,3);
		font::AlphanumericsPrint(font::ALIGN_CENTER, .06, -.1, -.22, &pen, "%c", '_');

		pen = (mEditCurrentPos == 1) ? vector::pen(1,1,1,1,3) : vector::pen(1,1,1,.2,3);
		font::AlphanumericsPrint(font::ALIGN_CENTER, .06, 0, -.22, &pen, "%c", '_');

		pen = (mEditCurrentPos == 2) ? vector::pen(1,1,1,1,3) : vector::pen(1,1,1,.2,3);
		font::AlphanumericsPrint(font::ALIGN_CENTER, .06, .1, -.22, &pen, "%c", '_');

	}

    pen = defaultFontPen;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .015, 0, -.6, &pen, "Use Left Stick to Select Letter\nUse Right Stick to Change Position\nPress Bomb Button When Done");
	pen.a=.1;
	pen.lineRadius = 8;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .015, 0, -.6, &pen, "Use Left Stick to Select Letter\nUse Right Stick to Change Position\nPress Bomb Button When Done");


}

void highscore::drawTable()
{
    vector::pen pen = defaultFontPen;
    font::AlphanumericsPrint(font::ALIGN_CENTER, .04, 0, .5, &pen, "High Scores");

	// Number
	float x = -.25;
	float y = .35;
	for (int i=0; i<10; i++)
	{
		vector::pen pen = defaultFontPen;
		font::AlphanumericsPrint(font::ALIGN_LEFT, .015, x, y, &pen, "%d.", i+1);
		y-= .07;
	}

	// Name
	x = -.02;
	y = .35;
	for (int i=0; i<10; i++)
	{
		vector::pen pen = defaultFontPen;
		font::AlphanumericsPrint(font::ALIGN_CENTER, .015, x, y, &pen, "%s", mEntries[i].name);
		y-= .07;
	}

	// Score
	x = .15;
	y = .35;
	for (int i=0; i<10; i++)
	{
        char format[512];
        sprintf(format, "%d", mEntries[i].score);
        char* s = font::formatStringWithCommas(format);

		vector::pen pen = defaultFontPen;
		font::AlphanumericsPrint(font::ALIGN_LEFT, .015, x, y, &pen, "%s", s);
		y-= .07;
	}

}

void highscore::load()
{
    FILE * pFile;
    pFile = fopen ("scores.sav","rb");
    if (pFile)
    {
        fread(mEntries, 1, sizeof(mEntries), pFile);
        fclose(pFile);
    }
}

void highscore::save()
{
    FILE * pFile;
    pFile = fopen ("scores.sav","wb");
    if (pFile)
    {
        fwrite(mEntries, 1, sizeof(mEntries), pFile);
        fclose(pFile);
    }
}



