/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/HardwareSupport/SpaceNavPollInterface.hh>
#include <iostream>
#include <string>

int g_SpaceNavListenerCount = 0;

using namespace std;

namespace ACGL{
namespace HardwareSupport{

//
// define ACGL_SPACE_NAVIGATOR_SUPPORT to build functions for the space nav support
//
#if defined(ACGL_SPACE_NAVIGATOR_SUPPORT)
#if defined(__gnu_linux__)
    #include <ACGL/HardwareSupport/mini_spnav.h>

    // the functions we need to load:
    spnav_open_ptr       spnav_open       = NULL;
    spnav_close_ptr      spnav_close      = NULL;
    spnav_poll_event_ptr spnav_poll_event = NULL;

    // pointer to the library:
    void *libspnav = NULL;

    #define SPACENAV_SPNAV_LINUX
#elif defined(__APPLE__) && defined (__MACH__) // MacOS X with official drivers
    #include <stack>
    #include <ConnexionClientAPI.h>
    #define SPACENAV_CONNEXION_MAC

    extern const char *__progname;
    UInt16 g_SpaceNavClientID;
#endif
#endif // dummy

#ifdef SPACENAV_SPNAV_LINUX
bool SpaceNavConnectToDriverLinux();
void SpaceNavDisconnectFromDriverLinux();
unsigned int SpaceNavPollEventLinux( SpaceNavEvent &eventToFill );
#elif defined(SPACENAV_CONNEXION_MAC)
bool SpaceNavConnectToDriverMac();
void SpaceNavDisconnectFromDriverMac();
unsigned int SpaceNavPollEventMac( SpaceNavEvent &eventToFill );
#endif

bool SpaceNavRegisterListener()
{
    if ( g_SpaceNavListenerCount == 0 ) {
        // init & connect to the driver:
        bool success = false;
#ifdef SPACENAV_SPNAV_LINUX
        success = SpaceNavConnectToDriverLinux();
#elif defined(SPACENAV_CONNEXION_MAC)
        success = SpaceNavConnectToDriverMac();
#else
        // unsupported OS: quietly(!) return false
        return false;
#endif
        if (!success)
        {
            ACGL::Utils::warning() << "Unable to connect to space navigator!" << std::endl;
            return false; // don't increase the listener count!
        }
        else
        {
            ACGL::Utils::debug() << "Successfully connected to space navigator!" << std::endl;
        }
    }
    g_SpaceNavListenerCount++;
    return true;
}

void SpaceNavUnregisterListener()
{
    if ( g_SpaceNavListenerCount == 0 ) {
        // can't unregister if there are no listeners!
        return;
    }
    g_SpaceNavListenerCount--;
    if ( g_SpaceNavListenerCount == 0 ) {
        // if there are now 0 listeners disconnect from the driver
#ifdef SPACENAV_SPNAV_LINUX
        SpaceNavDisconnectFromDriverLinux();
#elif defined(SPACENAV_CONNEXION_MAC)
        SpaceNavDisconnectFromDriverMac();
#endif
    }
}

unsigned int SpaceNavPollEvent( SpaceNavEvent &eventToFill )
{
    if ( g_SpaceNavListenerCount == 0 ) {
        // In this case there was no (successful) connection to the driver!
        // e.g. no drivers or no device is installed!
        return 0;
    }

#ifdef SPACENAV_SPNAV_LINUX
    return SpaceNavPollEventLinux( eventToFill );
#elif defined(SPACENAV_CONNEXION_MAC)
    return SpaceNavPollEventMac( eventToFill );
#else
    // unsupported OS:
    return 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SPACENAV_SPNAV_LINUX
bool SpaceNavConnectToDriverLinux()
{
    // try to open the library:
    libspnav = dlopen("libspnav.so", RTLD_NOW);
    if (libspnav == NULL) {
        std::cerr << "can't locate libspnav.so" << std::endl;
        return false;
    }

    // get the needed function pointers:
    spnav_open       = (spnav_open_ptr)       dlsym(libspnav, "spnav_open");
    spnav_close      = (spnav_close_ptr)      dlsym(libspnav, "spnav_close");
    spnav_poll_event = (spnav_poll_event_ptr) dlsym(libspnav, "spnav_poll_event");

    if (!spnav_open || !spnav_close || !spnav_poll_event) {
        std::cerr << "could not load function pointers from libspnav.so" << std::endl;
        return false;
    }

    int status = spnav_open();
    return ( status != -1 );
}

void SpaceNavDisconnectFromDriverLinux()
{
    spnav_close();
    if (libspnav) dlclose( libspnav );
}

unsigned int SpaceNavPollEventLinux( SpaceNavEvent &eventToFill )
{
    spnav_event event;
    unsigned int eventsWaiting = spnav_poll_event(&event);
    if ( eventsWaiting > 0 )
    {
        if (event.type == SPNAV_EVENT_MOTION )
        {
            eventToFill.type = movement;
            eventToFill.x =   event.motion.x;
            eventToFill.y =   event.motion.y;
            eventToFill.z =  -event.motion.z;
            eventToFill.rx =  event.motion.rx;
            eventToFill.ry = -event.motion.ry;
            eventToFill.rz =  event.motion.rz;
        } else if (event.type == SPNAV_EVENT_BUTTON ) {
            eventToFill.type    = button;
            eventToFill.button  = event.button.bnum;
            eventToFill.pressed = event.button.press;
        } else {
            eventToFill.type = other;
        }

        //spnav_remove_events(SPNAV_EVENT_MOTION);
        return eventsWaiting;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(SPACENAV_CONNEXION_MAC)

std::stack< SpaceNavEvent > g_SpaceNavEventList;

void SpaceNavCallbackMac( io_connect_t connection, natural_t messageType, void *messageArgument )
{
    //cout << "callback" << endl;
    static uint32_t oldButtonState = 0;

    ConnexionDeviceState *state;
    switch (messageType) {
        case kConnexionMsgDeviceState:
            state = (ConnexionDeviceState*)messageArgument;
            if (state->client == g_SpaceNavClientID)
            {
                SpaceNavEvent newEvent;
                // decipher what command/event is being reported by the driver
                switch (state->command) {
                    case kConnexionCmdHandleAxis:{
                        newEvent.type = movement;
                        // scale down all values to approximately match the linux values
                        newEvent.x =  state->axis[0] /=  2;
                        newEvent.y =  state->axis[1] /= -2;
                        newEvent.z =  state->axis[2] /= -2;
                        newEvent.rx = state->axis[3] /=  2;
                        newEvent.ry = state->axis[4] /=  2;
                        newEvent.rz = state->axis[5] /=  2;
                        g_SpaceNavEventList.push( newEvent );
                    }
                    break;
                    case kConnexionCmdHandleButtons: {
                        newEvent.type = button;
                        uint32_t newButtonState = state->buttons;
                        uint32_t buttonDiff = oldButtonState ^ newButtonState;

                        // on MacOS the buttons are set as a bitpattern representing
                        // the current state. Strangely only the first 7 buttons
                        // seem to work and it fails if many buttons are pressed at once!
                        for (int i = 0; i < 32; ++i) {
                            uint32_t mask = 1<<i;
                            if (buttonDiff & mask) {
                                bool pressed = (newButtonState & mask);
                                newEvent.pressed = pressed;
                                newEvent.button  = i;
                                g_SpaceNavEventList.push( newEvent );
                            }
                        }
                        //cout << "button: " << state->buttons << " " << buttonDiff << endl;

                        oldButtonState = newButtonState;
                        break;
                        }
                default:
                    newEvent.type = other;
                    g_SpaceNavEventList.push( newEvent );
                }
            }
            break;
        default:
            break;
    }
}

bool SpaceNavConnectToDriverMac()
{
    // get a pascal string of the app name:
    std::string progname = std::string( __progname );
    unsigned char *appname = new unsigned char[ progname.length() + 1 ];
    int len = progname.length();
    if (len > 255) {
        std::cerr << "app " << progname << " might be too long to support the space navigator" << std::endl;
        len = 255;
    }
    appname[0] = len;
    for (int i = 0; i < len; ++i) {
        appname[i+1] = progname[i];
    }

    OSErr result = InstallConnexionHandlers( SpaceNavCallbackMac, 0L, 0L);
    if (result != noErr) {
        return false;
    }
    g_SpaceNavClientID = RegisterConnexionClient( kConnexionClientWildcard, appname, kConnexionClientModeTakeOver, kConnexionMaskAll);

    delete[] appname;
    return true;
}

void SpaceNavDisconnectFromDriverMac()
{
    UnregisterConnexionClient( g_SpaceNavClientID );
    CleanupConnexionHandlers();
}

unsigned int SpaceNavPollEventMac( SpaceNavEvent &eventToFill )
{
    if (g_SpaceNavEventList.size() == 0) {
        return 0;
    }

    eventToFill = g_SpaceNavEventList.top();
    g_SpaceNavEventList.pop();

    // return the number of waiting events including the one just returned!
    return (g_SpaceNavEventList.size()+1);
}

#endif

} // HardwareSupport
} // ACGL
