#ifndef CONTROLS_H
#define CONTROLS_H

#include "point3d.h"

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

private:

    // Keyboard
    Point3d readKeyboardLeftStick(int player);
    Point3d readKeyboardRightStick(int player);
    bool readKeyboardTrigger(int player);
    bool readKeyboardStart(int player);

    // XBox controller
    Point3d readXBoxControllerLeftStick(int player);
    Point3d readXBoxControllerRightStick(int player);
    bool readXBoxControllerTrigger(int player);
    bool readXBoxStart(int player);

    SDL_Joystick* mControllers[4];

    int mNumJoysticks;

};

#endif // CONTROLS_H

