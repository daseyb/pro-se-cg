/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>
#include <string>
#include <linuxjoystick/Joystick.h>

namespace ACGL{
namespace HardwareSupport{

/**
 * NOTE: Will not work unless ACGL_COMPILE_WITH_GLFW or ACGL_COMPILE_WITH_QT (linux only) is defined!
 *
 *
 * For the known gamepads, all axes start at 0.0f. All trigger go from 0..1 and all joystick-like
 * analog sticks go from -1..1 while -1 is on the left or bottom / 1 is right or top.
 */
class GamePad {
public:
    enum GamePadButton {
        SELECT = 0,
        START  = 1,
        LEFT_PAD_NORTH  = 2,
        LEFT_PAD_EAST   = 3,
        LEFT_PAD_SOUTH  = 4,
        LEFT_PAD_WEST   = 5,
        RIGHT_PAD_NORTH = 6,
        RIGHT_PAD_EAST  = 7,
        RIGHT_PAD_SOUTH = 8,
        RIGHT_PAD_WEST  = 9,
        LEFT_SHOULDER   = 10,
        RIGHT_SHOULDER  = 11,
        LEFT_TRIGGER    = 12,
        RIGHT_TRIGGER   = 13,

        // number of elements in this enum (itself not included):
        GAMEPAD_BUTTON_ENUM_SIZE = 14
    };

    enum GamePadAxis {
        LEFT_ANALOG_TRIGGER  = 0, // 0..1
        RIGHT_ANALOG_TRIGGER = 1, // 0..1
        LEFT_ANALOG_STICK_X  = 2, // -1..1 aka left-right
        LEFT_ANALOG_STICK_Y  = 3, // -1..1 aka up-down / front-back
        RIGHT_ANALOG_STICK_X = 4, // -1..1 aka left-right
        RIGHT_ANALOG_STICK_Y = 5, // -1..1 aka up-down / front-back

        // number of elements in this enum (itself not included):
        GAMEPAD_AXIS_ENUM_SIZE = 6
    };


    //! connects to the _n-th joystick, start counting at 1!
    GamePad( int _n = 1 );
    ~GamePad();

    //! true if the joystick was found, note that unplugging the GamePad at runtime can not be detected!
    bool ok() { return mGamePadOK; }

    ///////////// Buttons

    //! true if the button with the internal number _button is pressed
    bool isPressedRaw( unsigned int _button );

    //! only returns true if the button was mapped first!
    bool isPressed( GamePadButton _button );

    //! true if the button was just pressed or released
    bool buttonStateChanged( unsigned int _button );

    //! true if the button was just pressed or released
    bool buttonStateChanged( GamePadButton _button );

    //! define the mapping of one button
    void setButtonMapping( GamePadButton _button, unsigned int _rawButtonNumber );

    ///////////// Axes

    //! analog sticks and analog trigger, values are from -1..1. An unknown axis will return 0.0
    float getAxisRaw( unsigned int _axis );

    float getAxis( GamePadAxis _axis );

    //! define the mapping of one button
    void setAxisMapping( GamePadAxis _axis, unsigned int _rawAxisNumber );

    //! sets the minimal value an axis has to be pushed to trigger.
    //! _sensitivity has to be >= 0.0 and < 1.0.
    //! reported axis values will still be between -1..0..1
    void setMinAxisSensitivity( float _sensitivity );

    float getMinAxisSensitivity() { return mMinSensitivity; }

    //! set and unset the invertation of an axis (1..-1 -> -1..1)
    void invertAxis( int _axis, bool _invert = true );

    //! print the button and axes state for debugging:
    void printState();

    //! print the names of all mapped buttons, in all caps if pressed
    void printPressedButtons();

    //! fetches the current device state, will do nothing if the joystick is not ok
    void update();

private:
    bool  mGamePadOK;
    int   mNumberOfButtons;
    int   mNumberOfAxes;
    int   mButtonMap[GAMEPAD_BUTTON_ENUM_SIZE];
    int   mAxisMap[GAMEPAD_AXIS_ENUM_SIZE];
    unsigned char *mButtonState; // 0 = released, 1 = pressed
    unsigned char *mLastButtonState;
    float *mAxes; // -1..1 (e.g. analog sticks) or 0..1 (analog trigger) per axis - untouched should be 0
    float *mAxesMultiplier; // used for scaling from the used API to -1..1
    float *mAxesAdd;        // also used for scaling
    float mMinSensitivity;  // a minimal axis value that has to be exceeded before it is evaluated

    std::string mJoystickName;

    // fills mAxis and mButtonState and also stores the old state
    void getAxisAndButtonValues();

    //
    // GLFW specifics: replace this to support joysticks with other APIs
    //
#ifdef ACGL_COMPILE_WITH_GLFW
    const unsigned char *mGLFWButtons; // temp. used as GLFW needs a const array
    const float         *mGLFWAxes;    // temp. used as GLFW needs a const array
    int mGLFWGamePadNumber;
#endif

    //
    // Custom Linux Joystick support:
    //
#ifdef ACGL_OWN_LINUX_JOYSTICK
    Joystick mLinuxGamePad;
#endif

    bool isMapped( GamePadButton _button );
    void printPressedButtonHelper( GamePadButton _b, const char *_TRUE, const char *_false );

	//
	// HACK for trigger mapping on windows of xbox 360 gamepad:
	//
	bool isXBox360OnWindows;
};
ACGL_SMARTPOINTER_TYPEDEFS(GamePad)

}
}

