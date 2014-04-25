#include "menuSelectGameType.h"


static int selection = 0;
static bool debounceStart = false;

static float player1Amount = 0;
static float mpPlayer1Amount = 0;
static float mpPlayer2Amount = 0;
static float mpPlayer3Amount = 0;
static float mpPlayer4Amount = 0;


void menuSelectGameType::init()
{
    debounceStart = true;
    player1Amount = 0;
    mpPlayer1Amount = 0;
    mpPlayer2Amount = 0;
    mpPlayer3Amount = 0;
    mpPlayer4Amount = 0;

    theGame.mPlayers.mPlayer1->mJoined = false;
    theGame.mPlayers.mPlayer2->mJoined = false;
    theGame.mPlayers.mPlayer3->mJoined = false;
    theGame.mPlayers.mPlayer4->mJoined = false;
}

void menuSelectGameType::run()
{
    int dir = 0;

    if (!debounceStart)
    {
        if (selection == 0)
        {
            if (theGame.mControls.getStartButton(0))
            {
                theGame.mPlayers.mPlayer1->mJoined = true;
                theGame.startGame(1, game::GAMETYPE_SINGLEPLAYER);
            }
        }
        else
        {
            if (theGame.mControls.getStartButton(0)) theGame.mPlayers.mPlayer1->mJoined = true;
            if (theGame.mControls.getStartButton(1)) theGame.mPlayers.mPlayer2->mJoined = true;
            if (theGame.mControls.getStartButton(2)) theGame.mPlayers.mPlayer3->mJoined = true;
            if (theGame.mControls.getStartButton(3)) theGame.mPlayers.mPlayer4->mJoined = true;

            if (theGame.mControls.getTriggerButton(0)
                || theGame.mControls.getTriggerButton(1)
                || theGame.mControls.getTriggerButton(2)
                || theGame.mControls.getTriggerButton(3))
            {
                theGame.startGame(theGame.numPlayers(), game::GAMETYPE_MULTIPLAYER_COOP);
            }

/*
            if (theGame.mControls.getStartButton(0)
                || theGame.mControls.getStartButton(1)
                || theGame.mControls.getStartButton(2)
                || theGame.mControls.getStartButton(3))
            {
                //theGame.startGame(4, game::GAMETYPE_MULTIPLAYER_COOP);

                if (theGame.mControls.getStartButton(0))

            }
*/

    /*
            if (theGame.mControls.getStartButton(0))
            {
                theGame.startGame(1, game::GAMETYPE_SINGLEPLAYER);
            }
            else if (theGame.mControls.getStartButton(1))
            {
                theGame.startGame(2, game::GAMETYPE_MULTIPLAYER_COOP);
            }
            else if (theGame.mControls.getStartButton(2))
            {
                theGame.startGame(3, game::GAMETYPE_MULTIPLAYER_COOP);
            }
            else if (theGame.mControls.getStartButton(3))
            {
                theGame.startGame(4, game::GAMETYPE_MULTIPLAYER_COOP);
            }
    */
        }
    }
    else
    {
        if (!theGame.mControls.getStartButton(0)
            && !theGame.mControls.getStartButton(1)
            && !theGame.mControls.getStartButton(2)
            && !theGame.mControls.getStartButton(3))
        {
            debounceStart = false;
        }
    }

    Point3d leftStick = game::mControls.getLeftStick(0);
    float leftDistance = mathutils::calculate2dDistance(Point3d(0,0,0), leftStick);
    if (leftDistance > .1)
    {
        if (leftStick.x < -.5)
        {
            dir = 1;
        }
        else if (leftStick.x > .5)
        {
            dir = -1;
        }
    }
    Point3d rightStick = game::mControls.getRightStick(0);
    float rightDistance = mathutils::calculate2dDistance(Point3d(0,0,0), rightStick);
    if (rightDistance > .1)
    {
        if (rightStick.x < -.5)
        {
            dir = 1;
        }
        else if (rightStick.x > .5)
        {
            dir = -1;
        }
    }

    if (dir < 0) selection = 0;
    if (dir > 0) selection = 1;
/*
    if (dir > 0)
    {
        ++selection;
        if (selection > 1)
        {
            selection = 1;
        }
    }
    else if (dir < 0)
    {
        --selection;
        if (selection < 0)
        {
            selection = 0;
        }
    }
*/
    const float amountIncrease = .008;
    const float amountDampener = .992;

    if (selection == 0)
    {
        if (player1Amount < 1)
        {
            player1Amount += amountIncrease;
        }
        else if (player1Amount > 1)
        {
            player1Amount *= amountDampener;
        }

        mpPlayer1Amount *= amountDampener;
        mpPlayer2Amount *= amountDampener;
        mpPlayer3Amount *= amountDampener;
        mpPlayer4Amount *= amountDampener;
    }
    else if (selection == 1)
    {
        float player1Target = theGame.mPlayers.mPlayer1->mJoined ? 1 : 0;
        float player2Target = theGame.mPlayers.mPlayer2->mJoined ? 1 : 0;
        float player3Target = theGame.mPlayers.mPlayer3->mJoined ? 1 : 0;
        float player4Target = theGame.mPlayers.mPlayer4->mJoined ? 1 : 0;

        if (mpPlayer1Amount < player1Target)
        {
            mpPlayer1Amount += amountIncrease;
        }
        else if (mpPlayer1Amount > player1Target)
        {
            mpPlayer1Amount *= amountDampener;
        }

        if (mpPlayer2Amount < player2Target)
        {
            mpPlayer2Amount += amountIncrease;
        }
        else if (mpPlayer2Amount > player2Target)
        {
            mpPlayer2Amount *= amountDampener;
        }

        if (mpPlayer3Amount < player3Target)
        {
            mpPlayer3Amount += amountIncrease;
        }
        else if (mpPlayer3Amount > player3Target)
        {
            mpPlayer3Amount *= amountDampener;
        }

        if (mpPlayer4Amount < player4Target)
        {
            mpPlayer4Amount += amountIncrease;
        }
        else if (mpPlayer4Amount > player4Target)
        {
            mpPlayer4Amount *= amountDampener;
        }

        player1Amount *= amountDampener;
    }
}

void menuSelectGameType::draw()
{
    static float angle0=88, angle1=123, angle2=321, angle3=424, angle4=242;
    angle0 += .052;
    angle1 += .051;
    angle2 += .049;
    angle3 += .05;
    angle4 += .052;

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    Point3d pos;
    pos.x = theGame.mGrid.extentX() / 2;
    pos.y = theGame.mGrid.extentY() / 2;
    pos.z = 0;

    glLineWidth(4);

    // Single player

    pos.x = (theGame.mGrid.extentX() / 2)+0;
    pos.y = (theGame.mGrid.extentY() / 2)+16;

    {
        vector::pen pen = theGame.mPlayers.mPlayer1->getPen();
        pen.r = 1;
        pen.g = 1;
        pen.b = 1;
        glBegin(GL_LINES);
        theGame.mPlayers.mPlayer1->getModel()->Identity();
        theGame.mPlayers.mPlayer1->getModel()->Scale(4);
        theGame.mPlayers.mPlayer1->getModel()->Rotate((sin(angle0)*.5) * player1Amount);
        theGame.mPlayers.mPlayer1->getModel()->Translate(pos);
        theGame.mPlayers.mPlayer1->getModel()->emit(pen);
        glEnd();
    }


    // Multiplayer

    pos.x = (theGame.mGrid.extentX() / 2)-30;
    pos.y = (theGame.mGrid.extentY() / 2)-16;

    {
        vector::pen pen = vector::pen(1, .3, .3, 1, 12); // HACK - find a better way to get this color :-(
        if (!theGame.mPlayers.mPlayer1->mJoined) pen.a *= .25;
        glBegin(GL_LINES);
        theGame.mPlayers.mPlayer1->getModel()->Identity();
        theGame.mPlayers.mPlayer1->getModel()->Scale(4);
        theGame.mPlayers.mPlayer1->getModel()->Rotate((sin(angle1)*.5) * mpPlayer1Amount);
        theGame.mPlayers.mPlayer1->getModel()->Translate(pos);
        theGame.mPlayers.mPlayer1->getModel()->emit(pen);
        glEnd();
    }

    pos.x = (theGame.mGrid.extentX() / 2)-10;
    pos.y = (theGame.mGrid.extentY() / 2)-16;

    {
        vector::pen pen = theGame.mPlayers.mPlayer2->getPen();
        if (!theGame.mPlayers.mPlayer2->mJoined) pen.a *= .25;
        glBegin(GL_LINES);
        theGame.mPlayers.mPlayer2->getModel()->Identity();
        theGame.mPlayers.mPlayer2->getModel()->Scale(.25);
        theGame.mPlayers.mPlayer2->getModel()->Rotate((sin(angle2)*.5) * mpPlayer2Amount);
        theGame.mPlayers.mPlayer2->getModel()->Translate(pos);
        theGame.mPlayers.mPlayer2->getModel()->emit(pen);
        glEnd();
    }


    pos.x = (theGame.mGrid.extentX() / 2)+10;
    pos.y = (theGame.mGrid.extentY() / 2)-16;

    {
        vector::pen pen = theGame.mPlayers.mPlayer3->getPen();
        if (!theGame.mPlayers.mPlayer3->mJoined) pen.a *= .25;
        glBegin(GL_LINES);
        theGame.mPlayers.mPlayer3->getModel()->Identity();
        theGame.mPlayers.mPlayer3->getModel()->Scale(.25);
        theGame.mPlayers.mPlayer3->getModel()->Rotate((sin(angle3)*.5) * mpPlayer3Amount);
        theGame.mPlayers.mPlayer3->getModel()->Translate(pos);
        theGame.mPlayers.mPlayer3->getModel()->emit(pen);
        glEnd();
    }

    pos.x = (theGame.mGrid.extentX() / 2)+30;
    pos.y = (theGame.mGrid.extentY() / 2)-16;

    {
        vector::pen pen = theGame.mPlayers.mPlayer4->getPen();
        if (!theGame.mPlayers.mPlayer4->mJoined) pen.a *= .25;
        glBegin(GL_LINES);
        theGame.mPlayers.mPlayer4->getModel()->Identity();
        theGame.mPlayers.mPlayer4->getModel()->Scale(.25);
        theGame.mPlayers.mPlayer4->getModel()->Rotate((sin(angle4)*.5) * mpPlayer4Amount);
        theGame.mPlayers.mPlayer4->getModel()->Translate(pos);
        theGame.mPlayers.mPlayer4->getModel()->emit(pen);
        glEnd();
    }

    // Single player selection rectangle
    if (selection == 0)
    {
        glColor4f(1,1,1,1);
        glLineWidth(4);

        glBegin(GL_LINE_LOOP);

        Point3d p1((theGame.mGrid.extentX() / 2)-10, (theGame.mGrid.extentY() / 2)+25, 0);
        Point3d p2((theGame.mGrid.extentX() / 2)+10, (theGame.mGrid.extentY() / 2)+25, 0);
        Point3d p3((theGame.mGrid.extentX() / 2)+10, (theGame.mGrid.extentY() / 2)+7, 0);
        Point3d p4((theGame.mGrid.extentX() / 2)-10, (theGame.mGrid.extentY() / 2)+7, 0);

        glVertex3f(p1.x, p1.y, 0 );
        glVertex3f(p2.x, p2.y, 0 );
        glVertex3f(p3.x, p3.y, 0 );
        glVertex3f(p4.x, p4.y, 0 );

        glEnd();
    }

    // Multi player selection rectangle
    if (selection == 1)
    {
        glColor4f(1,1,1,1);
        glLineWidth(4);

        glBegin(GL_LINE_LOOP);

        Point3d p1((theGame.mGrid.extentX() / 2)-40, (theGame.mGrid.extentY() / 2)-25, 0);
        Point3d p2((theGame.mGrid.extentX() / 2)+40, (theGame.mGrid.extentY() / 2)-25, 0);
        Point3d p3((theGame.mGrid.extentX() / 2)+40, (theGame.mGrid.extentY() / 2)-7, 0);
        Point3d p4((theGame.mGrid.extentX() / 2)-40, (theGame.mGrid.extentY() / 2)-7, 0);

        glVertex3f(p1.x, p1.y, 0 );
        glVertex3f(p2.x, p2.y, 0 );
        glVertex3f(p3.x, p3.y, 0 );
        glVertex3f(p4.x, p4.y, 0 );

        glEnd();
    }

    glDisable(GL_MULTISAMPLE);
    glDisable(GL_LINE_SMOOTH);
}