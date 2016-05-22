/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>

namespace ACGL{
namespace HardwareSupport{

/*
 * To compile:
 *  Linux: do nothing
 *  MacOS X: link to 3DconnexionClient,
 *           e.g. add 'SET(LIBRARIES -Wl,-framework,3DconnexionClient)' to CMakeLists.txt
 *           define ACGL_SPACE_NAVIGATOR_SUPPORT
 *  Windows: only dummy functionality will get build
 */

// Each listener should call this *once* before starting to poll,
// the first listener will start a connection to the driver.
// If the connection could be made, true will be returned, false otherwise.
// If false gets returned, SpaceNavUnregisterListener() should not get called
// and all SpaceNavPollEvent() calls will return 0 events.
bool SpaceNavRegisterListener();

// Each listener should call this *once* when stopping to listen iff the call
// to SpaceNavRegisterListener() returned true.
// Last unregister call will trigger disconnection from the driver.
void SpaceNavUnregisterListener();

enum SpaceNavEventType {
    movement,
    button,
    other
};

// Values of the buttons:
// the 2 button space navigator:
static const int SNE_BUTTON_LEFT  = 0;
static const int SNE_BUTTON_RIGHT = 1;
// the larger space pilot:
static const int SNE_BUTTON_1 = 0;
static const int SNE_BUTTON_2 = 1;
static const int SNE_BUTTON_3 = 2;
static const int SNE_BUTTON_4 = 3;
static const int SNE_BUTTON_5 = 4;
static const int SNE_BUTTON_6 = 5;
static const int SNE_BUTTON_ESC    = 10;
static const int SNE_BUTTON_CTRL   = 13;
static const int SNE_BUTTON_ALT    = 11;
static const int SNE_BUTTON_SHIFT  = 12;
static const int SNE_BUTTON_CONFIG = 20;
static const int SNE_BUTTON_PANEL  = 15;
static const int SNE_BUTTON_VOL_DOWN = 17;
static const int SNE_BUTTON_VOL_UP   = 16;
static const int SNE_BUTTON_DOM      = 18;
static const int SNE_BUTTON_T = 6;
static const int SNE_BUTTON_L = 7;
static const int SNE_BUTTON_R = 8;
static const int SNE_BUTTON_F = 9;
static const int SNE_BUTTON_3D  = 19;
static const int SNE_BUTTON_FIT = 14;

//
// translation and rotation can go from about -500 to 500.
// note that the neutral postion might not be 0,0,0
// not all axes have the same maximum value (-361 to 441 might be possible)
struct SpaceNavEvent {
    SpaceNavEventType type;
    int x,y,z;      // translation, right-handed-coord-system (right/up/towards the user are positive)
    int rx,ry,rz;   // rotation, pitch-yaw-roll
    int button;     // button number, see values above
    bool pressed;   // button state, pressing the button down and release will create two events
};

// Call this to poll for new events.
// Return value is the number of valid events waiting, 0 means no event
// is waiting and the eventToFill will not get changed in that situation.
// If no device was found or the connection to the driver failed for other
// reasons, 0 will get returned.
// -> polling for events and only interpreting them if >0 gets returned
//    should be always save!
unsigned int SpaceNavPollEvent( SpaceNavEvent &eventToFill );

} // HardwareSupport
} // ACGL
