#include "controls.h"
#include "mathutils.h"
#include "scene.h"

#define AXIS_MAX 32768

const float CLAMPVALUE = .3;

controls::controls()
{
    OutputDebugString(L"Initing controls\n");

    // Look for an xbox controller to use
    mNumJoysticks = SDL_NumJoysticks();

    TCHAR s[256];
    wsprintf(s, L"Found %d joysticks\n", mNumJoysticks);
    OutputDebugString(s);

    mControllers[0] = NULL;
    mControllers[1] = NULL;
    mControllers[2] = NULL;
    mControllers[3] = NULL;

    if (mNumJoysticks>=1)
    {
        // If only one joystick is connected, what the hell?
        // Let the one joystick control all 4 players :-)
        mControllers[0] = SDL_JoystickOpen(0);
        mControllers[1] = mControllers[0];
        mControllers[2] = mControllers[0];
        mControllers[3] = mControllers[0];
    }
    if (mNumJoysticks>=2)
    {
        mControllers[1] = SDL_JoystickOpen(1);
    }
    if (mNumJoysticks>=3)
    {
        mControllers[2] = SDL_JoystickOpen(2);
    }
    if (mNumJoysticks>=4)
    {
        mControllers[3] = SDL_JoystickOpen(3);
    }
}

controls::~controls()
{
    for (int i=0; i<mNumJoysticks; i++)
    {
        SDL_JoystickClose(mControllers[i]);
    }
}

Point3d controls::getLeftStick(int player)
{
    return readKeyboardLeftStick(player) + readXBoxControllerLeftStick(player);
}

Point3d controls::getRightStick(int player)
{
    return readKeyboardRightStick(player) + readXBoxControllerRightStick(player);
}

bool controls::getTriggerButton(int player)
{
    return readKeyboardTrigger(player) || readXBoxControllerTrigger(player);
}

bool controls::getStart1Button()
{
    return readKeyboardStart1();
}

bool controls::getStart2Button()
{
    return readKeyboardStart2();
}

//
// Keyboard controller
//
Point3d controls::readKeyboardLeftStick(int player)
{
    bool up = (::GetAsyncKeyState('W') & 0x8000);
    bool down = (::GetAsyncKeyState('S') & 0x8000);
    bool left = (::GetAsyncKeyState('A') & 0x8000);
    bool right = (::GetAsyncKeyState('D') & 0x8000);

    int x = 0;
    int y = 0;

    x += up ? 1:0;
    x -= down ? 1:0;

    y += left ? 1:0;
    y -= right ? 1:0;

    return Point3d(x,y,0);
}

Point3d controls::readKeyboardRightStick(int player)
{
    bool up = (::GetAsyncKeyState(VK_UP) & 0x8000);
    bool down = (::GetAsyncKeyState(VK_DOWN) & 0x8000);
    bool left = (::GetAsyncKeyState(VK_LEFT) & 0x8000);
    bool right = (::GetAsyncKeyState(VK_RIGHT) & 0x8000);

    int x = 0;
    int y = 0;

    x += up ? 1:0;
    x -= down ? 1:0;

    y += left ? 1:0;
    y -= right ? 1:0;

    return Point3d(x,y,0);
}

bool controls::readKeyboardTrigger(int player)
{
    return (::GetAsyncKeyState(VK_SPACE) & 0x8000);
}

bool controls::readKeyboardStart1()
{
    return (::GetAsyncKeyState('1') & 0x8000);
}

bool controls::readKeyboardStart2()
{
    return (::GetAsyncKeyState('2') & 0x8000);
}

bool controls::readKeyboardStart3()
{
    return (::GetAsyncKeyState('3') & 0x8000);
}

bool controls::readKeyboardStart4()
{
    return (::GetAsyncKeyState('4') & 0x8000);
}


//
// XBox controller
//

Point3d controls::readXBoxControllerLeftStick(int player)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    Point3d vector;
    vector.x = -(float)(SDL_JoystickGetAxis(mControllers[player], 1)) / AXIS_MAX;
    vector.y = -(float)(SDL_JoystickGetAxis(mControllers[player], 0)) / AXIS_MAX;
    vector.z = 0;

    if (fabs(vector.x) < CLAMPVALUE)
    {
        vector.x = 0;
    }
    if (fabs(vector.y) < CLAMPVALUE)
    {
        vector.y = 0;
    }

    return vector;
}

Point3d controls::readXBoxControllerRightStick(int player)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    Point3d vector;
    vector.x = -(float)(SDL_JoystickGetAxis(mControllers[player], 3)) / AXIS_MAX;
    vector.y = -(float)(SDL_JoystickGetAxis(mControllers[player], 4)) / AXIS_MAX;
    vector.z = 0;

    if (fabs(vector.x) < CLAMPVALUE)
    {
        vector.x = 0;
    }
    if (fabs(vector.y) < CLAMPVALUE)
    {
        vector.y = 0;
    }

    return vector;
}

bool controls::readXBoxControllerTrigger(int player)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    float triggerVal = (float)(SDL_JoystickGetAxis(mControllers[player], 2)) / AXIS_MAX;

    if (fabs(triggerVal) > CLAMPVALUE)
    {
        return true;
    }

    return false;
}
