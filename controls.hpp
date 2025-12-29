#pragma once

#include "point3d.hpp"

#include "SDL.h"

class controls
{
  public:
    controls();
    ~controls();

    Point3d getLeftStick(int player);
    Point3d getRightStick(int player);
    bool getTriggerButton(int player);
    bool getStartButton(int player);
    bool getBackButton(int player);
    bool getPauseButton(int player);

    // Hot-plugging support
    void handleControllerAdded(int deviceIndex);
    void handleControllerRemoved(SDL_JoystickID instanceId);
    void scanForControllers();

  private:
    // Keyboard
    Point3d readKeyboardLeftStick(int player);
    Point3d readKeyboardRightStick(int player);
    bool readKeyboardTrigger(int player);
    bool readKeyboardStart(int player);
    bool readKeyboardBack(int player);
    bool readKeyboardPause(int player);

    // Controllers (modern API)
    Point3d readControllerLeftStick(int player);
    Point3d readControllerRightStick(int player);
    bool readControllerTrigger(int player);
    bool readControllerStart(int player);
    bool readControllerBack(int player);
    bool readControllerPause(int player);

    SDL_GameController* mControllers[4];
    int mNumControllers;
};
