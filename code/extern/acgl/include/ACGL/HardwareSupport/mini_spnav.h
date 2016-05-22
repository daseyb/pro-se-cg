/*
This file is based on a part of libspnav, part of the spacenav project (spacenav.sf.net)
Original file (spnav.h)  Copyright (C) 2007-2010 John Tsiombikas <nuclear@member.fsf.org>
This file (mini_spnav.h) Copyright (C) 2013 Robert Menzel

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/
#ifndef MINI_SPACENAV_H_
#define MINI_SPACENAV_H_

/*
 * This file can be used as an alternative to spnav.h iff the library should get loaded
 * dynamically. Add this to your project and the code will compile independent of the
 * presence of libspnav (and also run independent of it).
 */

#include <dlfcn.h> // for dlopen

enum {
    SPNAV_EVENT_ANY,	/* used by spnav_remove_events() */
    SPNAV_EVENT_MOTION,
    SPNAV_EVENT_BUTTON	/* includes both press and release */
};

struct spnav_event_motion {
    int type;
    int x, y, z;
    int rx, ry, rz;
    unsigned int period;
    int *data;
};

struct spnav_event_button {
    int type;
    int press;
    int bnum;
};

typedef union spnav_event {
    int type;
    struct spnav_event_motion motion;
    struct spnav_event_button button;
} spnav_event;


#ifdef __cplusplus
extern "C" {
#endif

/* Open connection to the daemon via AF_UNIX socket.
 * The unix domain socket interface is an alternative to the original magellan
 * protocol, and it is *NOT* compatible with the 3D connexion driver. If you wish
 * to remain compatible, use the X11 protocol (spnav_x11_open, see below).
 * Returns -1 on failure.
 */
typedef int (*spnav_open_ptr)(void);

/* Close connection to the daemon. Use it for X11 or AF_UNIX connections.
 * Returns -1 on failure
 */
typedef int (*spnav_close_ptr)(void);

/* Retrieves the file descriptor used for communication with the daemon, for
 * use with select() by the application, if so required.
 * If the X11 mode is used, the socket used to communicate with the X server is
 * returned, so the result of this function is always reliable.
 * If AF_UNIX mode is used, the fd of the socket is returned or -1 if
 * no connection is open / failure occured.
 */
//int spnav_fd(void);

/* TODO: document */
//int spnav_sensitivity(double sens);

/* blocks waiting for space-nav events. returns 0 if an error occurs */
//int spnav_wait_event(spnav_event *event);

/* checks the availability of space-nav events (non-blocking)
 * returns the event type if available, or 0 otherwise.
 */
typedef int (*spnav_poll_event_ptr)(spnav_event *event);

/* Removes any pending events from the specified type, or all pending events
 * events if the type argument is SPNAV_EVENT_ANY. Returns the number of
 * removed events.
 */
//int spnav_remove_events(int type);


#ifdef __cplusplus
}
#endif

#endif	/* MINI_SPACENAV_H_ */
