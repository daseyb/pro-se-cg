/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/HardwareSupport/GamePad.hh>
#include <ACGL/Utils/Log.hh>
#include <ACGL/Math/Math.hh>
#include <cassert>
#include <cstring>

#ifdef ACGL_COMPILE_WITH_GLFW
#include <GLFW/glfw3.h>
#endif

#if !defined(ACGL_COMPILE_WITH_GLFW) && !defined(ACGL_OWN_LINUX_JOYSTICK)
#define ACGL_NO_GAMEPAD_SUPPORT
#endif

using namespace std;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::HardwareSupport;

//! connects to the _n-th joystick
GamePad::GamePad( int _n )
{
    assert( _n != 0 );

#ifdef ACGL_NO_GAMEPAD_SUPPORT
    assert( 0 && "compiled without any gamepad supporting library, gamepad will always report that no buttons are pressed" );
    warning() << "compiled without any gamepad supporting library, gamepad will always report that no buttons are pressed" << endl;
#endif

    mGamePadOK = false;
    mAxes            = NULL;
    mAxesMultiplier  = NULL;
    mAxesAdd         = NULL;
    mMinSensitivity  = 0.0f;
    mButtonState     = NULL;
    mLastButtonState = NULL;
	isXBox360OnWindows = false;

#if defined( ACGL_COMPILE_WITH_GLFW )
    int numberOfJoysticksFound = 0;
    for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
        if (glfwJoystickPresent(i) == GL_TRUE) {
            numberOfJoysticksFound++;

            if ( numberOfJoysticksFound == _n) {
                mGamePadOK = true;
                mGLFWGamePadNumber = i;
                break;
            }
        }
    }
#elif defined( ACGL_OWN_LINUX_JOYSTICK )
    int jsNumber = _n - 1; // the joysticks in /dev/input start counting at 0!
    if (jsNumber != 0) {
        error() << "only gamepad 0 is supported right now" << std::endl;
    }
    mGamePadOK = mLinuxGamePad.open( "/dev/input/js0" );
#endif

    if (!mGamePadOK) {
        mNumberOfButtons = 0;
        mNumberOfAxes    = 0;
#ifdef ACGL_COMPILE_WITH_GLFW
        mGLFWButtons = NULL;
        mGLFWAxes    = NULL;
        mGLFWGamePadNumber = 0;
#endif
    } else {
        // gamepad was found, get number of buttons/axes
#if defined(ACGL_COMPILE_WITH_GLFW)
        mGLFWButtons = glfwGetJoystickButtons( mGLFWGamePadNumber, &mNumberOfButtons );
        mGLFWAxes    = glfwGetJoystickAxes(    mGLFWGamePadNumber, &mNumberOfAxes );

#elif defined( ACGL_OWN_LINUX_JOYSTICK )
        mNumberOfButtons = mLinuxGamePad.getNumberOfButtons();
        mNumberOfAxes    = mLinuxGamePad.getNumberOfAxes();
#endif
    }

    //
    // init state arrays:
    //
    mLastButtonState = new unsigned char[mNumberOfButtons];
    mButtonState     = new unsigned char[mNumberOfButtons];
    mAxes            = new float[mNumberOfAxes];
    mAxesMultiplier  = new float[mNumberOfAxes];
    mAxesAdd         = new float[mNumberOfAxes];

    for (int i = 0; i < mNumberOfButtons; ++i) {
        mButtonState[i] = 0;
    }
    for (int i = 0; i < mNumberOfAxes; ++i) {
        mAxesMultiplier[i] = 1.0f;
        mAxesAdd[i] = 0.0f;
        mAxes[i]    = 0.0f;
    }

    //
    // prepare button mapping
    //
    for (int i = 0; i < GAMEPAD_BUTTON_ENUM_SIZE; ++i) {
        // set to an invalid button number so it will always get reported as false:
        mButtonMap[i] = GAMEPAD_BUTTON_ENUM_SIZE;
    }
    for (int i = 0; i < GAMEPAD_AXIS_ENUM_SIZE; ++i) {
        // set to an invalid button number so it will always get reported as false:
        mAxisMap[i] = GAMEPAD_AXIS_ENUM_SIZE;
    }

    mJoystickName = "UNKNOWN";
    if (mGamePadOK) {
#if defined( ACGL_COMPILE_WITH_GLFW )
        mJoystickName = string( glfwGetJoystickName(mGLFWGamePadNumber) );
#elif defined( ACGL_OWN_LINUX_JOYSTICK )
        mJoystickName = mLinuxGamePad.getIdentifier();
#endif
        debug() << "Gamepad name: " << mJoystickName << std::endl;
    }

    if (mGamePadOK) {
        //                     Linux name                                              MacOS X name
        if ((mJoystickName == "Sony PLAYSTATION(R)3 Controller") || (mJoystickName == "PLAYSTATION(R)3 Controller")) {
            setButtonMapping( SELECT, 0);
            setButtonMapping( START,  3);
            setButtonMapping( LEFT_PAD_NORTH  , 4);
            setButtonMapping( LEFT_PAD_EAST   , 5);
            setButtonMapping( LEFT_PAD_SOUTH  , 6);
            setButtonMapping( LEFT_PAD_WEST   , 7);
            setButtonMapping( RIGHT_PAD_NORTH , 12);
            setButtonMapping( RIGHT_PAD_EAST  , 13);
            setButtonMapping( RIGHT_PAD_SOUTH , 14);
            setButtonMapping( RIGHT_PAD_WEST  , 15);
            setButtonMapping( LEFT_SHOULDER   , 10);
            setButtonMapping( RIGHT_SHOULDER  , 11);
            setButtonMapping( LEFT_TRIGGER    , 8);
            setButtonMapping( RIGHT_TRIGGER   , 9);

            setAxisMapping( LEFT_ANALOG_TRIGGER  , 12); mAxesMultiplier[12] =  0.5f; mAxesAdd[12] = 0.5f;
            setAxisMapping( RIGHT_ANALOG_TRIGGER , 13); mAxesMultiplier[13] = -0.5f; mAxesAdd[13] = 0.5f;
            setAxisMapping( LEFT_ANALOG_STICK_X  , 0);
            setAxisMapping( LEFT_ANALOG_STICK_Y  , 1);
            setAxisMapping( RIGHT_ANALOG_STICK_X , 2);
            setAxisMapping( RIGHT_ANALOG_STICK_Y , 3);

            setMinAxisSensitivity( 0.05f );
		} else if (mJoystickName == "Microsoft X-Box 360 pad") { // 360 USB gamepad on Linux

            // real buttons:
            setButtonMapping( SELECT, 6);
            setButtonMapping( START,  7);
            setButtonMapping( RIGHT_PAD_NORTH , 3);
            setButtonMapping( RIGHT_PAD_EAST  , 1);
            setButtonMapping( RIGHT_PAD_SOUTH , 0);
            setButtonMapping( RIGHT_PAD_WEST  , 2);
            setButtonMapping( LEFT_SHOULDER   , 4);
            setButtonMapping( RIGHT_SHOULDER  , 5);

            // virtual buttons (analog axis values interpreted as buttons):
            // (some buttons on the xbox controller are actually analog axes)
            setButtonMapping( LEFT_PAD_NORTH  , 7+mNumberOfButtons); // axis 7
            setButtonMapping( LEFT_PAD_EAST   , 6+mNumberOfButtons); // axis 6
            setButtonMapping( LEFT_PAD_SOUTH  , 7+mNumberOfButtons+mNumberOfAxes); // axis 7 inverse
            setButtonMapping( LEFT_PAD_WEST   , 6+mNumberOfButtons+mNumberOfAxes); // axis 6 inverse
            setButtonMapping( LEFT_TRIGGER    , 2+mNumberOfButtons); // axis 2
            setButtonMapping( RIGHT_TRIGGER   , 5+mNumberOfButtons); // axis 5

            setAxisMapping( LEFT_ANALOG_TRIGGER  , 2); mAxesMultiplier[2] =  0.5f; mAxesAdd[2] = 0.5f;
            setAxisMapping( RIGHT_ANALOG_TRIGGER , 5); mAxesMultiplier[5] = -0.5f; mAxesAdd[5] = 0.5f;
            setAxisMapping( LEFT_ANALOG_STICK_X  , 0);
            setAxisMapping( LEFT_ANALOG_STICK_Y  , 1);
            setAxisMapping( RIGHT_ANALOG_STICK_X , 3); mAxesMultiplier[3] = -1.0f;
            setAxisMapping( RIGHT_ANALOG_STICK_Y , 4); mAxesMultiplier[4] = -1.0f;

            setMinAxisSensitivity( 0.2f );


		} else if (mJoystickName == "Microsoft PC-joystick driver") { // 360 USB gamepad on Windows

			// real buttons:
			setButtonMapping(SELECT, 6);
			setButtonMapping(START,  7);
			setButtonMapping(RIGHT_PAD_NORTH, 3);
			setButtonMapping(RIGHT_PAD_EAST,  1);
			setButtonMapping(RIGHT_PAD_SOUTH, 0);
			setButtonMapping(RIGHT_PAD_WEST,  2);
			setButtonMapping(LEFT_SHOULDER,   4);
			setButtonMapping(RIGHT_SHOULDER,  5);
			setButtonMapping(LEFT_PAD_NORTH, 10);
			setButtonMapping(LEFT_PAD_EAST,  11);
			setButtonMapping(LEFT_PAD_SOUTH, 12);
			setButtonMapping(LEFT_PAD_WEST,  13);


			isXBox360OnWindows = true;

			setAxisMapping(LEFT_ANALOG_STICK_X, 0);
			setAxisMapping(LEFT_ANALOG_STICK_Y, 1);
			setAxisMapping(RIGHT_ANALOG_STICK_Y, 3); mAxesMultiplier[3] = -1.0f;
			setAxisMapping(RIGHT_ANALOG_STICK_X, 4);

			setMinAxisSensitivity(0.2f);


		} else {
            debug() << "unknown gamepad: " << mJoystickName << " can't configure buttons" << endl;
        }
    }
}

GamePad::~GamePad()
{
#if defined( ACGL_COMPILE_WITH_GLFW )

#elif defined( ACGL_OWN_LINUX_JOYSTICK )
    mLinuxGamePad.close();
#endif
    delete[] mAxes;
    delete[] mAxesMultiplier;
    delete[] mAxesAdd;
    delete[] mButtonState;
    delete[] mLastButtonState;
}

bool GamePad::isPressedRaw( unsigned int _button )
{
    if ( (int)_button < mNumberOfButtons) {
        // it's a button:
        return ( mButtonState[_button] == 1 );
    }
    // "buttons" from mNumberOfButtons to (mNumberOfButtons+mNumberOfAxes-1) are
    // pushed if the axis value is >= 0.5.
    int axis = (int)_button - mNumberOfButtons;
    if (axis < mNumberOfAxes) {
        return (mAxes[axis] >= 0.5f);
    }
    // "buttons" from (mNumberOfButtons+mNumberOfAxes) to (mNumberOfButtons+2*mNumberOfAxes-1) are
    // pushed if the axis value is <= -0.5.
    axis = axis - mNumberOfAxes;
    if (axis < mNumberOfAxes) {
        return (mAxes[axis] <= -0.5f);
    }
    // else the input was just too high, so return false
    return false;
}

bool GamePad::isPressed( GamePadButton _button )
{
    return isPressedRaw( mButtonMap[_button] );
}

void GamePad::setButtonMapping( GamePadButton _button, unsigned int _rawButtonNumber )
{
    assert(_button < GAMEPAD_BUTTON_ENUM_SIZE);
    mButtonMap[_button] = _rawButtonNumber;
}

bool GamePad::buttonStateChanged( unsigned int _button )
{
    if ( (int)_button > mNumberOfButtons) return false;

    return (mButtonState[_button] != mLastButtonState[_button]);
}


bool GamePad::buttonStateChanged( GamePadButton _button )
{
    return buttonStateChanged( mButtonMap[_button] );
}

float GamePad::getAxisRaw( unsigned int _axis )
{
    if ( (int)_axis > mNumberOfAxes) return 0.0f;
    return mAxes[_axis];
}

float GamePad::getAxis( GamePadAxis _axis )
{
	if (isXBox360OnWindows) {
		if (_axis == GamePad::LEFT_ANALOG_TRIGGER) {
			return std::max(getAxisRaw(2), 0.0f);
		}
		else if (_axis == GamePad::RIGHT_ANALOG_TRIGGER) {
			return -std::min(getAxisRaw(2), 0.0f);
		}
	}
    return getAxisRaw( mAxisMap[_axis] );
}

void GamePad::setAxisMapping( GamePadAxis _axis, unsigned int _rawAxisNumber )
{
    assert(_axis < GAMEPAD_AXIS_ENUM_SIZE);
    mAxisMap[_axis] = _rawAxisNumber;
}

void GamePad::setMinAxisSensitivity( float _sensitivity )
{
    assert( _sensitivity >= 0.0f && "sensitivity can't be negative" );
    assert( _sensitivity <  1.0f && "sensitivity has to be smaller than one" );

    mMinSensitivity = _sensitivity;
}

void GamePad::invertAxis( int _axis, bool _invert )
{
    assert(_axis < GAMEPAD_AXIS_ENUM_SIZE);
    mAxesMultiplier[_axis] = (_invert)? -1.0f : 1.0f;
}

void GamePad::getAxisAndButtonValues()
{
    //
    // get fresh state:
#ifdef ACGL_COMPILE_WITH_GLFW
    mGLFWButtons = glfwGetJoystickButtons( mGLFWGamePadNumber, &mNumberOfButtons );
    mGLFWAxes    = glfwGetJoystickAxes(    mGLFWGamePadNumber, &mNumberOfAxes );
    memcpy( mButtonState, mGLFWButtons, mNumberOfButtons );
    memcpy( mAxes,        mGLFWAxes,    mNumberOfAxes   * sizeof(float) );
#elif defined( ACGL_OWN_LINUX_JOYSTICK )

    EventJoystick event;
    while (mLinuxGamePad.pollEventButton( event )) {
        //debug() << "B time " << event.time << " value " << event.value << " number " << event.number << " synthetic " << event.synthetic << endl;
        //debug() << "B value " << event.value << " number " << (int)event.number << " synthetic " << event.synthetic << endl;

        if (event.synthetic == 0) {
            mButtonState[ event.number ] = (unsigned char) event.value;
        }

    }
    while (mLinuxGamePad.pollEventAxis( event )) {
        //debug() << "A time " << event.time << " value " << event.value << " number " << event.number << " synthetic " << event.synthetic << endl;
        //debug() << "A value " << event.value << " number " << (int)event.number << " synthetic " << event.synthetic << endl;

        if (event.synthetic == 0) {
            mAxes[ event.number ] = (float) event.value / 32767.0f; // values a bit over 1, but this will later be clamped
        }
    }
#else
    for (int i = 0; i < mNumberOfButtons; ++i) {
        mButtonState[i] = 0;
    }
    for (int i = 0; i < mNumberOfAxes; ++i) {
        mAxes[i] = 0.0f;
    }
#endif

    for (int i = 0; i < mNumberOfAxes; ++i) {
        mAxes[i] = mAxes[i] * mAxesMultiplier[i] + mAxesAdd[i];
    }
}

void GamePad::update()
{
    if (!ok()) return;

    //
    // store old button state
    // old axis state is not stored as this changes nearly constantly anyway
    memcpy( mLastButtonState, mButtonState, mNumberOfButtons ); // arrays are unsigned char

    //
    // get raw state:
    getAxisAndButtonValues();

    //
    // scale the axes:
    for (int i = 0; i < mNumberOfAxes; ++i) {
        float tmp = std::abs( mAxes[ i ] );
        tmp -= mMinSensitivity;
        if (tmp < 0.0f) tmp = 0.0f;
        tmp /= (1.0f - mMinSensitivity); // rescaled to 0..1
        tmp = glm::clamp( tmp, 0.0f, 1.0f );
        float sign = ((mAxes[ i ] < 0.0f) && (tmp != 0.0f)) ? -1.0f : 1.0f;
        mAxes[ i ] = tmp * sign;
    }
	//debug().unmute();
	//printState();
	//debug().mute();
}

void GamePad::printState()
{
#ifdef ACGL_NO_GAMEPAD_SUPPORT
    warning() << "compiled without any gamepad supporting library, gamepad will always report that no buttons are pressed" << endl;
    return;
#endif

    if (!ok()) {
        debug() << "no gamepad found - restarting the application might be needed after plugging in a gamepad" << endl;
        return;
    }

    for (int i = 0; i < mNumberOfButtons; ++i) {
        debug() << (int) mButtonState[i] << " ";
    }
    debug() << "| ";

    std::stringstream axes;
    axes.precision(2);
    axes.setf( std::ios::fixed );
    for (int i = 0; i < mNumberOfAxes; ++i) {
        axes << mAxes[i] << "  ";
    }
    debug() << axes.str() << endl;
}

bool GamePad::isMapped( GamePadButton /*_button*/ )
{
    for (int i = 0; i < GAMEPAD_BUTTON_ENUM_SIZE; ++i)
    {
        if (mButtonMap[i] != GAMEPAD_BUTTON_ENUM_SIZE) return true;
    }
    return false;
}

void GamePad::printPressedButtonHelper( GamePadButton _b, const char *_TRUE, const char *_false )
{
    if (isMapped( _b )) {
        if (isPressed( _b )) {
            debug() << _TRUE << " ";
        } else {
            debug() << _false << " ";
        }
    }
}

void GamePad::printPressedButtons()
{
    printPressedButtonHelper( SELECT, "SELECT", "select" );
    printPressedButtonHelper( START, "START", "start" );
    printPressedButtonHelper( LEFT_PAD_NORTH,  "L_PAD_N", "l_pad_n");
    printPressedButtonHelper( LEFT_PAD_EAST,   "L_PAD_E", "l_pad_e");
    printPressedButtonHelper( LEFT_PAD_SOUTH,  "L_PAD_S", "l_pad_s");
    printPressedButtonHelper( LEFT_PAD_WEST,   "L_PAD_W", "l_pad_w");
    printPressedButtonHelper( RIGHT_PAD_NORTH, "R_PAD_N", "r_pad_n");
    printPressedButtonHelper( RIGHT_PAD_EAST,  "R_PAD_E", "r_pad_e");
    printPressedButtonHelper( RIGHT_PAD_SOUTH, "R_PAD_S", "r_pad_s");
    printPressedButtonHelper( RIGHT_PAD_WEST,  "R_PAD_W", "r_pad_w");
    printPressedButtonHelper( LEFT_SHOULDER,   "L_SHOULDER", "l_shoulder");
    printPressedButtonHelper( RIGHT_SHOULDER,  "R_SHOULDER", "r_shoulder");
    printPressedButtonHelper( LEFT_TRIGGER,    "L_TRIGGER", "l_trigger");
    printPressedButtonHelper( RIGHT_TRIGGER,   "R_TRIGGER", "r_trigger");
    debug() << endl;
}
