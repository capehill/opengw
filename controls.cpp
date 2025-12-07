#include "controls.hpp"
#include "mathutils.hpp"
#include "scene.hpp"

#include <SDL.h>

#include <algorithm>
#include <cstdio>

extern const Uint8* keyboardState; // TODO: maybe a getter function rather?

// XBOX JOYSTICK VALUES

constexpr float AXIS_MAX = 32768.f;
constexpr float CLAMPVALUE = .3f;

#define XBOX_BUTTON_A     0
#define XBOX_BUTTON_B     1
#define XBOX_BUTTON_X     2
#define XBOX_BUTTON_Y     3
#define XBOX_BUTTON_BACK  6
#define XBOX_BUTTON_START 7

// XBOX JOYSTICK VALUES

controls::controls()
{
    // Look for an xbox controller to use
    mNumJoysticks = SDL_NumJoysticks();

    printf("Initing controls\n");
    printf("Found %d joysticks\n", mNumJoysticks);

    mNumJoysticks = std::max(mNumJoysticks, 4);

    mControllers[0] = NULL;
    mControllers[1] = NULL;
    mControllers[2] = NULL;
    mControllers[3] = NULL;

    // If only one joystick is connected, what the hell?
    // Let the one joystick control all 4 players :-)

    for (int j = 0; j < mNumJoysticks; j++) {
        mControllers[j] = SDL_JoystickOpen(j);
    }
}

controls::~controls()
{
    for (int i = 0; i < mNumJoysticks; i++) {
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

bool controls::getStartButton(int player)
{
    return readKeyboardStart(player) || readXBoxStart(player);
}

bool controls::getBackButton(int player)
{
    return readKeyboardBack(player) || readXBoxBack(player);
}

bool controls::getPauseButton(int player)
{
    return readKeyboardPause(player) || readXBoxPause(player);
}

//
// Keyboard controller
//
Point3d controls::readKeyboardLeftStick(int /*player*/)
{
    bool up = keyboardState[SDL_SCANCODE_W];
    bool down = keyboardState[SDL_SCANCODE_S];
    bool left = keyboardState[SDL_SCANCODE_A];
    bool right = keyboardState[SDL_SCANCODE_D];

    int x = 0;
    int y = 0;

    x += up ? 1 : 0;
    x -= down ? 1 : 0;

    y += left ? 1 : 0;
    y -= right ? 1 : 0;

    return Point3d(x, y, 0);
}

Point3d controls::readKeyboardRightStick(int /*player*/)
{
    bool up = keyboardState[SDL_SCANCODE_UP];
    bool down = keyboardState[SDL_SCANCODE_DOWN];
    bool left = keyboardState[SDL_SCANCODE_LEFT];
    bool right = keyboardState[SDL_SCANCODE_RIGHT];

    int x = 0;
    int y = 0;

    x += up ? 1 : 0;
    x -= down ? 1 : 0;

    y += left ? 1 : 0;
    y -= right ? 1 : 0;

    return Point3d(x, y, 0);
}

bool controls::readKeyboardTrigger(int /*player*/)
{
    return keyboardState[SDL_SCANCODE_SPACE];
}

bool controls::readKeyboardStart(int player)
{
    switch (player) {
    case 0:
        return keyboardState[SDL_SCANCODE_1];
        break;
    case 1:
        return keyboardState[SDL_SCANCODE_2];
        break;
    case 2:
        return keyboardState[SDL_SCANCODE_3];
        break;
    case 3:
        return keyboardState[SDL_SCANCODE_4];
        break;
    }
    return false;
}

bool controls::readKeyboardBack(int /*player*/)
{
    return keyboardState[SDL_SCANCODE_BACKSPACE];
}

bool controls::readKeyboardPause(int /*player*/)
{
    return keyboardState[SDL_SCANCODE_P];
}

//
// XBox controller
//

Point3d controls::readXBoxControllerLeftStick(int player)
{
    if (!mControllers[player])
        return Point3d(0, 0, 0);

    Point3d vector;
    vector.x = -(SDL_JoystickGetAxis(mControllers[player], 1)) / AXIS_MAX;
    vector.y = -(SDL_JoystickGetAxis(mControllers[player], 0)) / AXIS_MAX;
    vector.z = 0;

    if (fabs(vector.x) < CLAMPVALUE) {
        vector.x = 0;
    }
    if (fabs(vector.y) < CLAMPVALUE) {
        vector.y = 0;
    }

    return vector;
}

Point3d controls::readXBoxControllerRightStick(int player)
{
    if (!mControllers[player])
        return Point3d(0, 0, 0);

    Point3d vector;
    vector.x = -(SDL_JoystickGetAxis(mControllers[player], 3)) / AXIS_MAX;
    vector.y = -(SDL_JoystickGetAxis(mControllers[player], 4)) / AXIS_MAX;
    vector.z = 0;

    if (fabs(vector.x) < CLAMPVALUE) {
        vector.x = 0;
    }
    if (fabs(vector.y) < CLAMPVALUE) {
        vector.y = 0;
    }

    return vector;
}

bool controls::readXBoxControllerTrigger(int player)
{
    if (!mControllers[player])
        return false;

    float triggerVal = (SDL_JoystickGetAxis(mControllers[player], 2)) / AXIS_MAX;

    if (fabs(triggerVal) > CLAMPVALUE) {
        return true;
    }

    return false;
}

bool controls::readXBoxStart(int player)
{
    if (!mControllers[player])
        return false;

    return SDL_JoystickGetButton(mControllers[player], XBOX_BUTTON_A);
}

bool controls::readXBoxBack(int player)
{
    if (!mControllers[player])
        return false;

    return SDL_JoystickGetButton(mControllers[player], XBOX_BUTTON_B);
}

bool controls::readXBoxPause(int player)
{
    if (!mControllers[player])
        return false;

    return SDL_JoystickGetButton(mControllers[player], XBOX_BUTTON_START);
}
