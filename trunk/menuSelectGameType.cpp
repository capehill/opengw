#include "menuSelectGameType.h"


int menuSelectGameType::selection = 0;

static bool debounceStart = false;

static float player1Amount = 0;
static float mpPlayer1Amount = 0;
static float mpPlayer2Amount = 0;
static float mpPlayer3Amount = 0;
static float mpPlayer4Amount = 0;


void menuSelectGameType::init(int player)
{
    debounceStart = true;
    player1Amount = 0;
    mpPlayer1Amount = 0;
    mpPlayer2Amount = 0;
    mpPlayer3Amount = 0;
    mpPlayer4Amount = 0;

    theGame.mPlayers.mPlayer1->mJoined = player == 0 ? true : false;
    theGame.mPlayers.mPlayer2->mJoined = player == 1 ? true : false;
    theGame.mPlayers.mPlayer3->mJoined = player == 2 ? true : false;
    theGame.mPlayers.mPlayer4->mJoined = player == 3 ? true : false;

    game::mSound.playTrack(SOUNDID_MENU_SELECT);
}

void menuSelectGameType::run()
{
    int dir = 0;

    if (theGame.mControls.getBackButton(0) || theGame.mControls.getBackButton(1) || theGame.mControls.getBackButton(2) || theGame.mControls.getBackButton(3))
    {
        // Exit the menu
        game::mGameMode = game::GAMEMODE_ATTRACT;
        game::mSound.playTrack(SOUNDID_MENU_SELECT);
        return;
    }

    if (!debounceStart)
    {
        if (selection == 0)
        {
            if (theGame.mControls.getStartButton(0) || theGame.mControls.getStartButton(1) || theGame.mControls.getStartButton(2) || theGame.mControls.getStartButton(3))
            {
                theGame.startGame(game::GAMETYPE_SINGLEPLAYER);
                game::mSound.playTrack(SOUNDID_MENU_SELECT);
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
                theGame.startGame(game::GAMETYPE_MULTIPLAYER_COOP);
                game::mSound.playTrack(SOUNDID_MENU_SELECT);
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

    Point3d leftStick = game::mControls.getLeftStick(0) + game::mControls.getLeftStick(1) + game::mControls.getLeftStick(2) + game::mControls.getLeftStick(3);
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

    Point3d rightStick = game::mControls.getRightStick(0) + game::mControls.getRightStick(1) + game::mControls.getRightStick(2) + game::mControls.getRightStick(3);
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

    if (dir < 0)
    {
        if (selection != 0)
        {
            selection = 0;
            game::mSound.playTrack(SOUNDID_MENU_SELECT);
        }
    }
    else if (dir > 0)
    {
        if (selection != 1)
        {
            selection = 1;
            game::mSound.playTrack(SOUNDID_MENU_SELECT);
        }
    }

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
    const float amountIncrease = .02;
    const float amountDampener = .97;

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

        if (mpPlayer1Amount > 0) mpPlayer1Amount *= amountDampener;
        if (mpPlayer2Amount > 0) mpPlayer2Amount *= amountDampener;
        if (mpPlayer3Amount > 0) mpPlayer3Amount *= amountDampener;
        if (mpPlayer4Amount > 0) mpPlayer4Amount *= amountDampener;
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

        if (player1Amount > 0) player1Amount *= amountDampener;
    }

    if (fabs(player1Amount) < amountIncrease) player1Amount = 0;
    if (fabs(mpPlayer1Amount) < amountIncrease) mpPlayer1Amount = 0;
    if (fabs(mpPlayer2Amount) < amountIncrease) mpPlayer2Amount = 0;
    if (fabs(mpPlayer3Amount) < amountIncrease) mpPlayer3Amount = 0;
    if (fabs(mpPlayer4Amount) < amountIncrease) mpPlayer4Amount = 0;

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
        pen.a = .5;
        glBegin(GL_LINES);
        theGame.mPlayers.mPlayer1->getModel()->Identity();
        theGame.mPlayers.mPlayer1->getModel()->Scale(4);
        theGame.mPlayers.mPlayer1->getModel()->Rotate((sin(angle0)*.5) * player1Amount);
        theGame.mPlayers.mPlayer1->getModel()->Translate(pos);
        theGame.mPlayers.mPlayer1->getModel()->emit(pen);
        glEnd();
    }


    // Multiplayer

    const float player1xPos = (theGame.mGrid.extentX() / 2)-30;
    const float player2xPos = (theGame.mGrid.extentX() / 2)-10;
    const float player3xPos = (theGame.mGrid.extentX() / 2)+10;
    const float player4xPos = (theGame.mGrid.extentX() / 2)+30;

    // PLAYER 1

    pos.x = player1xPos;
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

    // PLAYER 2

    pos.x = player2xPos;
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

    // PLAYER 3

    pos.x = player3xPos;
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

    // PLAYER 4

    pos.x = player4xPos;
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
        glColor4f(1,1,1,.5);
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

        Point3d p1((theGame.mGrid.extentX() / 2)+40, (theGame.mGrid.extentY() / 2)-7, 0);
        Point3d p2((theGame.mGrid.extentX() / 2)-40, (theGame.mGrid.extentY() / 2)-7, 0);
        Point3d p3((theGame.mGrid.extentX() / 2)-40, (theGame.mGrid.extentY() / 2)-25, 0);

        const float indicatorW = 3;
        const float indicatorH = 1.5;

        // Player 1 indicator
        Point3d p4(player1xPos-(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);
        Point3d p5(player1xPos, ((theGame.mGrid.extentY() / 2)-25) + ((theGame.mPlayers.mPlayer1->mJoined) ? indicatorH : 0), 0);
        Point3d p6(player1xPos+(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);

        // Player 2 indicator
        Point3d p7(player2xPos-(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);
        Point3d p8(player2xPos, ((theGame.mGrid.extentY() / 2)-25) + ((theGame.mPlayers.mPlayer2->mJoined) ? indicatorH : 0), 0);
        Point3d p9(player2xPos+(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);

        // Player 3 indicator
        Point3d p10(player3xPos-(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);
        Point3d p11(player3xPos, ((theGame.mGrid.extentY() / 2)-25) + ((theGame.mPlayers.mPlayer3->mJoined) ? indicatorH : 0), 0);
        Point3d p12(player3xPos+(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);

        // Player 4 indicator
        Point3d p13(player4xPos-(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);
        Point3d p14(player4xPos, ((theGame.mGrid.extentY() / 2)-25) + ((theGame.mPlayers.mPlayer4->mJoined) ? indicatorH : 0), 0);
        Point3d p15(player4xPos+(indicatorW/2), ((theGame.mGrid.extentY() / 2)-25), 0);

        Point3d p16((theGame.mGrid.extentX() / 2)+40, (theGame.mGrid.extentY() / 2)-25, 0);

        glVertex3f(p1.x, p1.y, 0 );
        glVertex3f(p2.x, p2.y, 0 );
        glVertex3f(p3.x, p3.y, 0 );
        glVertex3f(p4.x, p4.y, 0 );
        glVertex3f(p5.x, p5.y, 0 );
        glVertex3f(p6.x, p6.y, 0 );
        glVertex3f(p7.x, p7.y, 0 );
        glVertex3f(p8.x, p8.y, 0 );
        glVertex3f(p9.x, p9.y, 0 );
        glVertex3f(p10.x, p10.y, 0 );
        glVertex3f(p11.x, p11.y, 0 );
        glVertex3f(p12.x, p12.y, 0 );
        glVertex3f(p13.x, p13.y, 0 );
        glVertex3f(p14.x, p14.y, 0 );
        glVertex3f(p15.x, p15.y, 0 );
        glVertex3f(p16.x, p16.y, 0 );

        glEnd();
    }

    glDisable(GL_MULTISAMPLE);
    glDisable(GL_LINE_SMOOTH);
}