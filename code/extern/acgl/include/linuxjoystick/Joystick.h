/*
 * Based on http://andreasvolz.wordpress.com/2008/01/24/a-linux-joystick-class-for-c/
 * by Andreas Volz, released under the MIT license.
 * Ported to QT threads from Philip Trettner.
 *
 * Define ACGL_COMPILE_WITH_LINUX_JOYSTICK_SUPPORT to use this. Access the joystick/gamepads
 * using the ACGL wrapper HardwareSupport::GamePad .
 *
 *
Copyright (C) 2008 Andreas Volz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its Copyright notices. In addition publicly
documented acknowledgment must be given that this software has been used if no
source code of this software is made available publicly. This includes
acknowledgments in either Copyright notices, Manuals, Publicity and Marketing
documents or any documentation provided with any product containing this
software. This License does not apply to any software that links to the
libraries provided by this software (statically or dynamically), but only to
the software provided.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
//
// This joystick interface uses QT just for threading and only works on linux
// as it uses native linux APIs.
// Only use ACGL_OWN_LINUX_JOYSTICK to test if this gets used.
//
// TODO: get away from QT and use native C++11 threads to minimize dependencies.
//
#if defined(ACGL_COMPILE_WITH_QT) && defined(__linux__)
#define ACGL_OWN_LINUX_JOYSTICK
#endif

#ifdef ACGL_OWN_LINUX_JOYSTICK

#include <linux/joystick.h>

#include <string>
#include <queue>
#include <QMutex>
#include <QThread>

// for more info about the Linux Joystick API read
// /usr/src/linux/Documentation/input/joystick-api.txt

struct EventJoystick
{
  int32_t time;  // seconds till unix
  int16_t value; // -32k .. 32k
  int8_t number;
  bool synthetic;
};

// TODO: configurable joystick device; best a manager for autodetect...
class Joystick : QThread //: public sigc::trackable
{
public:
  //sigc::signal <void, const EventJoystick&> signalAxis;
  //sigc::signal <void, const EventJoystick&> signalButton;

  Joystick ();
  virtual ~Joystick ();

  /* Open a joystick device.
   * @param device A device (e.g. /dev/input/jsX).
   */
  bool open (const std::string &device);

  /* Close the joystick device.
   */
  bool close ();

  /*
   * @return Number of available buttons.
   * @return -1 Initializing not finished.
   */
  int getNumberOfButtons ();

  /*
   * @return Number of available axis.
   * @return -1 Initializing not finished.
   */
  int getNumberOfAxes ();

  /*
   * @return Identifier string of the Joystick
   */
  const std::string &getIdentifier ();

  /// polls one event
  /// Returns true if an event was found
  bool pollEventButton(EventJoystick &_event);
  bool pollEventAxis(EventJoystick &_event);

private: // intentionally not implemented
  Joystick             (const Joystick&);
  Joystick& operator = (const Joystick&);

private:
  std::queue<EventJoystick> mEventsBtn;
  std::queue<EventJoystick> mEventsAxis;

  QMutex mBtnMutex;
  QMutex mAxisMutex;

  struct js_event joy_event;
  int m_fd;
  bool m_init;
  int m_axes;
  int m_buttons;
  std::string m_name;
  bool m_run;

  void run ();
};

#endif // ACGL_OWN_LINUX_JOYSTICK
