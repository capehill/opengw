#include "font.h"
#include "game.h"
#include "point3d.h"
#include <stdio.h>
#include <stdarg.h>

float AlphanumericsPrintVScale = 1.0f;

static float cursorX = 0, cursorY = 0;
static const float xcell = 1.9;
static const float ycell = 3.5;

static float _topAscent = 0;
static float _bottomAscent = 0;

static float _topAscentOffset = 0;
static float _bottomAscentOffset = 0;

namespace font
{
    static inline void drawText(float scale, Point3d* p, vector::pen* color, BOOL lower=FALSE);

    static Point3d charA[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, .25, 0),

        Point3d(-1, .25, 0),
        Point3d(0, 1, 0),

        Point3d(0, 1, 0),
        Point3d(1, .25, 0),

        Point3d(1, .25, 0),
        Point3d(1, -1, 0),

        Point3d(-1, -.25, 0),
        Point3d(1, -.25, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charB[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(.66, 1, 0),

        Point3d(.66, 1, 0),
        Point3d(1, .66, 0),

        Point3d(1, .66, 0),
        Point3d(1, .33, 0),

        Point3d(1, .33, 0),
        Point3d(.5, 0, 0),

        Point3d(.5, 0, 0),
        Point3d(1, -.33, 0),

        Point3d(1, -.33, 0),
        Point3d(1, -.66, 0),

        Point3d(1, -.66, 0),
        Point3d(.66, -1, 0),

        Point3d(.66, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(.5, 0, 0),
        Point3d(-1, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charC[] =
    {
        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charD[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(.33, 1, 0),

        Point3d(.33, 1, 0),
        Point3d(1, .33, 0),

        Point3d(1, .33, 0),
        Point3d(1, -.33, 0),

        Point3d(1, -.33, 0),
        Point3d(.33, -1, 0),

        Point3d(.33, -1, 0),
        Point3d(-1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charE[] =
    {
        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(-1, 0, 0),
        Point3d(.5, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charF[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(-1, 0, 0),
        Point3d(.5, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charG[] =
    {

        Point3d(0, -.25, 0),
        Point3d(1, -.25, 0),

        Point3d(1, -.25, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, .25, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charH[] =
    {
        Point3d(-1, 1, 0),
        Point3d(-1, -1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charI[] =
    {
        Point3d(0, 1, 0),
        Point3d(0, -1, 0),

        Point3d(-.66, 1, 0),
        Point3d(.66, 1, 0),

        Point3d(-.66, -1, 0),
        Point3d(.66, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charJ[] =
    {
        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(.25, -1, 0),

        Point3d(.25, -1, 0),
        Point3d(-.66, -.33, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charK[] =
    {
        Point3d(-1, 1, 0),
        Point3d(-1, -1, 0),

        Point3d(1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charL[] =
    {
        Point3d(-1, 1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charM[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(0, 0, 0),

        Point3d(0, 0, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charN[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(1, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charO[] =
    {
        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charP[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(-1, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charQ[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -.33, 0),

        Point3d(1, -.33, 0),
        Point3d(0, -1, 0),

        Point3d(0, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(0, 0, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charR[] =
    {
        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charS[] =
    {
        Point3d(1, 1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charT[] =
    {
        Point3d(0, 1, 0),
        Point3d(0, -1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charU[] =
    {
        Point3d(-1, 1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(1, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charV[] =
    {
        Point3d(-1, 1, 0),
        Point3d(0, -1, 0),

        Point3d(0, -1, 0),
        Point3d(1, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charW[] =
    {
        Point3d(	-1,	 1 , 0),
        Point3d( -1,	-1 , 0),
        Point3d(	-1,	-1 , 0),
        Point3d( 0, -.2	, 0),
        Point3d(	 0,	-.2 , 0),
        Point3d( 1,	-1 , 0),
        Point3d(	 1,	-1 , 0),
        Point3d( 1,	1	, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charX[] =
    {
        Point3d(-1, 1, 0),
        Point3d(1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(1, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charY[] =
    {
        Point3d(-1, 1, 0),
        Point3d(0, 0, 0),

        Point3d(0, 0, 0),
        Point3d(1, 1, 0),

        Point3d(0, 0, 0),
        Point3d(0, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charZ[] =
    {
        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char0[] =
    {
        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),
        /*
    Point3d(0, .2, 0),
    Point3d(0, -.2, 0),
*/
        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char1[] =
    {
        Point3d(0, 1, 0),
        Point3d(0, -1, 0),

        Point3d(-.8, -1, 0),
        Point3d(.8, -1, 0),

        Point3d(0, 1, 0),
        Point3d(-.6, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d char2[] =
    {
        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char3[] =
    {
        Point3d(-1, -1, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(-1, 1, 0),

        Point3d(1, 0, 0),
        Point3d(-.5, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char4[] =
    {
        Point3d(-1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char5[] =
    {
        Point3d(1, 1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char6[] =
    {
        Point3d(1, 1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char7[] =
    {
        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char8[] =
    {
        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d char9[] =
    {

        Point3d(1, 0, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(1, -1, 0),

        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charLT[] =
    {
        Point3d(1, 1, 0),
        Point3d(-1, 0, 0),

        Point3d(-1, 0, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charGT[] =
    {
        Point3d(-1, 1, 0),
        Point3d(1, 0, 0),

        Point3d(1, 0, 0),
        Point3d(-1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charLBracket[] =
    {
        Point3d(.66, 1, 0),
        Point3d(0, 1, 0),

        Point3d(0, 1, 0),
        Point3d(0, -1, 0),

        Point3d(0, -1, 0),
        Point3d(.66, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charRBracket[] =
    {
        Point3d(-.66, 1, 0),
        Point3d(0, 1, 0),

        Point3d(0, 1, 0),
        Point3d(0, -1, 0),

        Point3d(0, -1, 0),
        Point3d(-.66, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charDegrees[] =
    {
        Point3d(-2, 1, 0),
        Point3d(-2.6, 1, 0),

        Point3d(-2.6, 1, 0),
        Point3d(-2.6, .4, 0),

        Point3d(-2.6, .4, 0),
        Point3d(-2, .4, 0),

        Point3d(-2, .4, 0),
        Point3d(-2, 1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charAsterisk[] =
    {
        Point3d(-1, 1, 0),
        Point3d(1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(1, 1, 0),

        Point3d(-1, 0, 0),
        Point3d(1, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charPeriod[] =
    {
        Point3d(0, -.6, 0),
        Point3d(0, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charExclamation[] =
    {
        Point3d(0, -.8, 0),
        Point3d(0, -1, 0),

        Point3d(0, 1, 0),
        Point3d(0, -.4, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charIExclamation[] =
    {
        Point3d(0, .8, 0),
        Point3d(0, 1, 0),

        Point3d(0, -1, 0),
        Point3d(0, .4, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charHyphen[] =
    {
        Point3d(-.8, 0, 0),
        Point3d(.8, 0, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charPlus[] =
    {
        Point3d(-.8, 0, 0),
        Point3d(.8, 0, 0),

        Point3d(0, -.5, 0),
        Point3d(0, .5, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };


    static Point3d charCopy[] =
    {
        Point3d(1, -1, 0),
        Point3d(-1, -1, 0),

        Point3d(-1, -1, 0),
        Point3d(-1, 1, 0),

        Point3d(-1, 1, 0),
        Point3d(1, 1, 0),

        Point3d(1, 1, 0),
        Point3d(1, -1, 0),


        Point3d(.5, -.5, 0),
        Point3d(-.5, -.5, 0),

        Point3d(-.5, -.5, 0),
        Point3d(-.5, .5, 0),

        Point3d(-.5, .5, 0),
        Point3d(.5, .5, 0),



        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charDoubleQuote[] =
    {
        Point3d(.5, 1, 0),
        Point3d(.5, 0, 0),

        Point3d(-.5, 1, 0),
        Point3d(-.5, 0, 0),


        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charSingleQuote[] =
    {
        Point3d(0, 1, 0),
        Point3d(0, .2, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charComma[] =
    {
        Point3d(-.8, -1.8, 0),
        Point3d(0, -.5, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    static Point3d charUnderscore[] =
    {
        Point3d(-1, -1, 0),
        Point3d(1, -1, 0),

        Point3d( -999,-999, -999), Point3d( -999,-999, -999 )
    };

    void AlphanumericsMeasureString(const char* string, float scale, float* width, float* height)
    {
        if (width)
        {
            *width = strlen(string)*scale*xcell;
            if (height) *height = scale*ycell;
        }
        else
        {
            if (height) *height = scale*ycell;
        }
    }

    int AlphanumericsGetNumLines(const char* string)
    {
        int lines = 1;

        int i;
        int len = strlen(string);
        for (i=0; i<len; i++)
        {
            if (string[i] == '\n')
            {
                ++lines;
            }
        }

        return lines;
    }

    int AlphanumericsGetLineLen(const char* string)
    {
        int lineLen = 0;

        while ((string[lineLen] != '\n') && (string[lineLen] != '\0'))
        {
            ++lineLen;
        };

        return lineLen;
    }

    void AlphanumericsSetAngle(float topAscent, float bottomAscent)
    {
        _topAscent = topAscent;
        _bottomAscent = bottomAscent;
    }

    void AlphanumericsPrint(int alignment, float scale, float x, float y, vector::pen* color, const char *fmt, ...)
    {
        //  return;

        if (fmt == NULL)
            return;

        glLineWidth(color->lineRadius);
        glColor4f(color->r, color->g, color->b, color->a);
//        glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINES);


        _topAscentOffset = 0;
        _bottomAscentOffset = 0;


        //
        // render the text
        //

        float		length=0;
        char		text[512];
        va_list		ap;

        va_start(ap, fmt);
        vsprintf(text, fmt, ap);
        va_end(ap);


        // Initial alignment
        if (alignment == ALIGN_LEFT)
        {
            cursorX = x;
        }
        if (alignment == ALIGN_CENTER)
        {
            float width = AlphanumericsGetLineLen(text) * xcell * scale;
            cursorX = x - (width/2);
            cursorX += (xcell*scale)/2;
        }

        cursorY = y;

        int len = strlen(text);
        for (int i=0; i<len; i++)
        {
            if (text[i] == '\n')
            {
                if (alignment == ALIGN_LEFT)
                {
                    cursorX = x;
                }
                if (alignment == ALIGN_CENTER)
                {
                    float width = AlphanumericsGetLineLen(&text[i+1]) * xcell * scale;
                    cursorX = x - (width/2);
                }

                cursorY-=ycell*scale;
            }
            else
            {
                BOOL lower = FALSE;

                switch (text[i])
                {
                case ' ':
                    break;
                case 'a':
                    lower = TRUE;
                case 'A':
                    drawText(scale, &charA[0], color, lower);
                    break;
                case 'b':
                    lower = TRUE;
                case 'B':
                    drawText(scale, &charB[0], color, lower);
                    break;
                case 'c':
                    lower = TRUE;
                case 'C':
                    drawText(scale, &charC[0], color, lower);
                    break;
                case 'd':
                    lower = TRUE;
                case 'D':
                    drawText(scale, &charD[0], color, lower);
                    break;
                case 'e':
                    lower = TRUE;
                case 'E':
                    drawText(scale, &charE[0], color, lower);
                    break;
                case 'f':
                    lower = TRUE;
                case 'F':
                    drawText(scale, &charF[0], color, lower);
                    break;
                case 'g':
                    lower = TRUE;
                case 'G':
                    drawText(scale, &charG[0], color, lower);
                    break;
                case 'h':
                    lower = TRUE;
                case 'H':
                    drawText(scale, &charH[0], color, lower);
                    break;
                case 'i':
                    lower = TRUE;
                case 'I':
                    drawText(scale, &charI[0], color, lower);
                    break;
                case 'j':
                    lower = TRUE;
                case 'J':
                    drawText(scale, &charJ[0], color, lower);
                    break;
                case 'k':
                    lower = TRUE;
                case 'K':
                    drawText(scale, &charK[0], color, lower);
                    break;
                case 'l':
                    lower = TRUE;
                case 'L':
                    drawText(scale, &charL[0], color, lower);
                    break;
                case 'm':
                    lower = TRUE;
                case 'M':
                    drawText(scale, &charM[0], color, lower);
                    break;
                case 'n':
                    lower = TRUE;
                case 'N':
                    drawText(scale, &charN[0], color, lower);
                    break;
                case 'o':
                    lower = TRUE;
                case 'O':
                    drawText(scale, &charO[0], color, lower);
                    break;
                case 'p':
                    lower = TRUE;
                case 'P':
                    drawText(scale, &charP[0], color, lower);
                    break;
                case 'q':
                    lower = TRUE;
                case 'Q':
                    drawText(scale, &charQ[0], color, lower);
                    break;
                case 'r':
                    lower = TRUE;
                case 'R':
                    drawText(scale, &charR[0], color, lower);
                    break;
                case 's':
                    lower = TRUE;
                case 'S':
                    drawText(scale, &charS[0], color, lower);
                    break;
                case 't':
                    lower = TRUE;
                case 'T':
                    drawText(scale, &charT[0], color, lower);
                    break;
                case 'u':
                    lower = TRUE;
                case 'U':
                    drawText(scale, &charU[0], color, lower);
                    break;
                case 'v':
                    lower = TRUE;
                case 'V':
                    drawText(scale, &charV[0], color, lower);
                    break;
                case 'w':
                    lower = TRUE;
                case 'W':
                    drawText(scale, &charW[0], color, lower);
                    break;
                case 'x':
                    lower = TRUE;
                case 'X':
                    drawText(scale, &charX[0], color, lower);
                    break;
                case 'y':
                    lower = TRUE;
                case 'Y':
                    drawText(scale, &charY[0], color, lower);
                    break;
                case 'z':
                    lower = TRUE;
                case 'Z':
                    drawText(scale, &charZ[0], color, lower);
                    break;
                case '0':
                    drawText(scale, &char0[0], color);
                    break;
                case '1':
                    drawText(scale, &char1[0], color);
                    break;
                case '2':
                    drawText(scale, &char2[0], color);
                    break;
                case '3':
                    drawText(scale, &char3[0], color);
                    break;
                case '4':
                    drawText(scale, &char4[0], color);
                    break;
                case '5':
                    drawText(scale, &char5[0], color);
                    break;
                case '6':
                    drawText(scale, &char6[0], color);
                    break;
                case '7':
                    drawText(scale, &char7[0], color);
                    break;
                case '8':
                    drawText(scale, &char8[0], color);
                    break;
                case '9':
                    drawText(scale, &char9[0], color);
                    break;

                case '<':
                    drawText(scale, &charLT[0], color);
                    break;
                case '>':
                    drawText(scale, &charGT[0], color);
                    break;

                case '[':
                    drawText(scale, &charLBracket[0], color);
                    break;
                case ']':
                    drawText(scale, &charRBracket[0], color);
                    break;

                case '*':
                    drawText(scale, &charAsterisk[0], color);
                    break;

                case '.':
                    drawText(scale, &charPeriod[0], color);
                    break;

                case '°':
                    drawText(scale, &charDegrees[0], color);
                    break;

                case '©':
                    drawText(scale, &charCopy[0], color);
                    break;

                case '\"':
                    drawText(scale, &charDoubleQuote[0], color);
                    break;

                case '\'':
                    drawText(scale, &charSingleQuote[0], color);
                    break;

                case ',':
                    drawText(scale, &charComma[0], color);
                    break;

                case '_':
                    drawText(scale, &charUnderscore[0], color);
                    break;

                case '!':
                    drawText(scale, &charExclamation[0], color);
                    break;

                case '^':
                    drawText(scale, &charIExclamation[0], color);
                    break;

                case '-':
                    drawText(scale, &charHyphen[0], color);
                    break;

                case '+':
                    drawText(scale, &charPlus[0], color);
                    break;

                }

                _topAscentOffset += _topAscent;
                _bottomAscentOffset += _bottomAscent;

                cursorX+=xcell*scale;
            }
        }

        glEnd();
        glDisable(GL_LINE_SMOOTH);

    }

    static inline void drawText(float scale, Point3d* p, vector::pen* color, BOOL lower)
    {
        Point3d start, end;

        while ((p->x != -999) && (p->y != -999))
        {
            float x1 = (p)->x;
            float y1 = (p++)->y;
            float x2 = (p)->x;
            float y2 = (p++)->y;

            start.x = x1;
            start.y = y1;
            end.x = x2;
            end.y = y2;


            float startXOff = start.x + 1;
            float startYOff = start.y + 1;

            float endXOff = end.x + 1;
            float endYOff = end.y + 1;


            if (start.x > 0)
                start.x += _topAscentOffset * (startXOff * startYOff);

            if (end.x > 0)
                end.x += _topAscentOffset * (endXOff * endYOff);


            start.x *= scale * .65;
            start.y *= scale * AlphanumericsPrintVScale * (lower ? .6 : 1.0);
            end.x *= scale * .65;
            end.y *= scale * AlphanumericsPrintVScale * (lower ? .6 : 1.0);

            if (!lower)
            {
                start.y += scale * .4;
                end.y += scale * .4;
            }

            start.x += cursorX;
            start.y += cursorY;
            end.x += cursorX;
            end.y += cursorY;

            glVertex3d(start.x, start.y, 0);
            glVertex3d(end.x, end.y, 0);

        };
    }

    // Converts a string "1234567" into "1,234,567"
    char* formatStringWithCommas(char* string)
    {
        static char buffer[512];

        int len = strlen(string); // yeah, not very safe
        int c=0;
        int dest = 0;
        for (int src=0; src<=len-1; )
        {
            if (c>0)
            {
                int fromEnd = len-c;
                if ((fromEnd%3) == 0)
                {
                    buffer[dest++] = ',';
                }
            }
            ++c;

            buffer[dest++] = string[src++];
        }
        buffer[dest++] = '\0';

        return buffer;
    }

};
