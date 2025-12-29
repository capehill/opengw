#include "controls.hpp"
#include "mathutils.hpp"
#include "scene.hpp"

#include <SDL.h>

#include <algorithm>
#include <cstdio>

extern const Uint8* keyboardState;

// CONTROLLER VALUES
constexpr float AXIS_MAX = 32768.f;
constexpr float CLAMPVALUE = .3f;

controls::controls()
{
    // Initialize the GameController subsystem
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        printf("Failed to initialize GameController subsystem: %s\n", SDL_GetError());
    }

    printf("Initing controls\n");

    for (int i = 0; i < 4; i++) {
        mControllers[i] = nullptr;
    }

    mNumControllers = 0;

    // Scan for initially connected controllers
    scanForControllers();
}

controls::~controls()
{
    for (int i = 0; i < 4; i++) {
        if (mControllers[i]) {
            SDL_GameControllerClose(mControllers[i]);
            mControllers[i] = nullptr;
        }
    }
}

void controls::scanForControllers()
{
    int numJoysticks = SDL_NumJoysticks();
    printf("Scanning for controllers... Found %d joysticks\n", numJoysticks);

    // Open any available game controllers
    for (int j = 0; j < numJoysticks && mNumControllers < 4; j++) {
        if (SDL_IsGameController(j)) {
            // Check if this device is already opened
            bool alreadyOpen = false;
            SDL_JoystickID joyId = SDL_JoystickGetDeviceInstanceID(j);

            for (int i = 0; i < 4; i++) {
                if (mControllers[i]) {
                    SDL_Joystick* joy = SDL_GameControllerGetJoystick(mControllers[i]);
                    if (SDL_JoystickInstanceID(joy) == joyId) {
                        alreadyOpen = true;
                        break;
                    }
                }
            }

            if (!alreadyOpen) {
                // Find first free slot
                for (int i = 0; i < 4; i++) {
                    if (!mControllers[i]) {
                        mControllers[i] = SDL_GameControllerOpen(j);
                        if (mControllers[i]) {
                            printf("Controller %d: %s\n", i,
                                   SDL_GameControllerName(mControllers[i]));
                            mNumControllers++;
                        }
                        break;
                    }
                }
            }
        }
    }

    printf("Total controllers opened: %d\n", mNumControllers);
}

void controls::handleControllerAdded(int deviceIndex)
{
    if (!SDL_IsGameController(deviceIndex))
        return;

    // Find first available slot
    for (int i = 0; i < 4; i++) {
        if (!mControllers[i]) {
            mControllers[i] = SDL_GameControllerOpen(deviceIndex);
            if (mControllers[i]) {
                printf("Controller connected in slot %d: %s\n", i,
                       SDL_GameControllerName(mControllers[i]));
                mNumControllers++;
            } else {
                printf("Failed to open controller %d: %s\n", deviceIndex, SDL_GetError());
            }
            break;
        }
    }
}

void controls::handleControllerRemoved(SDL_JoystickID instanceId)
{
    // Find and remove the disconnected controller
    for (int i = 0; i < 4; i++) {
        if (mControllers[i]) {
            SDL_Joystick* joy = SDL_GameControllerGetJoystick(mControllers[i]);
            if (SDL_JoystickInstanceID(joy) == instanceId) {
                printf("Controller disconnected from slot %d\n", i);
                SDL_GameControllerClose(mControllers[i]);
                mControllers[i] = nullptr;
                mNumControllers--;
                break;
            }
        }
    }
}

Point3d controls::getLeftStick(int player)
{
    return readKeyboardLeftStick(player) + readControllerLeftStick(player);
}

Point3d controls::getRightStick(int player)
{
    return readKeyboardRightStick(player) + readControllerRightStick(player);
}

bool controls::getTriggerButton(int player)
{
    return readKeyboardTrigger(player) || readControllerTrigger(player);
}

bool controls::getStartButton(int player)
{
    return readKeyboardStart(player) || readControllerStart(player);
}

bool controls::getBackButton(int player)
{
    return readKeyboardBack(player) || readControllerBack(player);
}

bool controls::getPauseButton(int player)
{
    return readKeyboardPause(player) || readControllerPause(player);
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
    case 1:
        return keyboardState[SDL_SCANCODE_2];
    case 2:
        return keyboardState[SDL_SCANCODE_3];
    case 3:
        return keyboardState[SDL_SCANCODE_4];
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
// Modern SDL GameController API
//

Point3d controls::readControllerLeftStick(int player)
{
    if (!mControllers[player])
        return Point3d(0, 0, 0);

    Point3d vector;
    // Note: Y axis is inverted for standard game controls
    vector.x = -(SDL_GameControllerGetAxis(mControllers[player], SDL_CONTROLLER_AXIS_LEFTY)) / AXIS_MAX;
    vector.y = -(SDL_GameControllerGetAxis(mControllers[player], SDL_CONTROLLER_AXIS_LEFTX)) / AXIS_MAX;
    vector.z = 0;

    // Apply deadzone
    if (fabs(vector.x) < CLAMPVALUE) {
        vector.x = 0;
    }
    if (fabs(vector.y) < CLAMPVALUE) {
        vector.y = 0;
    }

    return vector;
}

Point3d controls::readControllerRightStick(int player)
{
    if (!mControllers[player])
        return Point3d(0, 0, 0);

    Point3d vector;
    vector.x = -(SDL_GameControllerGetAxis(mControllers[player], SDL_CONTROLLER_AXIS_RIGHTY)) / AXIS_MAX;
    vector.y = -(SDL_GameControllerGetAxis(mControllers[player], SDL_CONTROLLER_AXIS_RIGHTX)) / AXIS_MAX;
    vector.z = 0;

    // Apply deadzone
    if (fabs(vector.x) < CLAMPVALUE) {
        vector.x = 0;
    }
    if (fabs(vector.y) < CLAMPVALUE) {
        vector.y = 0;
    }

    return vector;
}

bool controls::readControllerTrigger(int player)
{
    if (!mControllers[player])
        return false;

    // Right trigger
    float triggerVal = (SDL_GameControllerGetAxis(mControllers[player], SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / AXIS_MAX;

    if (triggerVal > CLAMPVALUE) {
        return true;
    }

    return false;
}

bool controls::readControllerStart(int player)
{
    if (!mControllers[player])
        return false;

    return SDL_GameControllerGetButton(mControllers[player], SDL_CONTROLLER_BUTTON_A);
}

bool controls::readControllerBack(int player)
{
    if (!mControllers[player])
        return false;

    return SDL_GameControllerGetButton(mControllers[player], SDL_CONTROLLER_BUTTON_B);
}

bool controls::readControllerPause(int player)
{
    if (!mControllers[player])
        return false;

    return SDL_GameControllerGetButton(mControllers[player], SDL_CONTROLLER_BUTTON_START);
}
