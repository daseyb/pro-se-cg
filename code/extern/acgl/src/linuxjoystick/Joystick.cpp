/*
 * See header for details and license.
 */

#include "linuxjoystick/Joystick.h"

#ifdef ACGL_OWN_LINUX_JOYSTICK

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

Joystick::Joystick () :
  m_init (false),
  m_axes (0),
  m_buttons (0),
  m_run (true)
{
}

Joystick::~Joystick ()
{
  this->close ();
}

bool Joystick::open (const string &device)
{
  m_fd = ::open (device.c_str(), O_RDONLY);
  if (m_fd == -1)
  {
    cerr << "Error opening joystick device!" << endl;

    return false;
  }
  else
  {
    char buttons;
    char axes;
    char name[128];

    // get number of buttons
    ioctl (m_fd, JSIOCGBUTTONS, &buttons);
    m_buttons = buttons;

    // get number of axes
    ioctl (m_fd, JSIOCGAXES, &axes);
    m_axes = axes;

    // get device name
    if (ioctl(m_fd, JSIOCGNAME (sizeof(name)), name) < 0)
    {
      m_name = "Unknown";
    }
    else
    {
      m_name = name;
    }

    /* TODO: support those if needed
     * #define JSIOCGVERSION   // get driver version
     * #define JSIOCSCORR      // set correction values
     * #define JSIOCGCORR      // get correction values
     */

    this->start();
    //thread = Glib::Thread::create (sigc::mem_fun (*this, &Joystick::loop), false);
    m_run = true;
  }

  return true;
}

bool Joystick::close ()
{
  // end thread
  m_run = false;

  // reset some values
  m_init = false;
  m_axes = 0;
  m_buttons = 0;

  return !::close (m_fd);
}

void Joystick::run ()
{
    this->setTerminationEnabled(true);
  // wait for all synthetic event until the first real event comes
  // then we've all available axis and buttons.

  while (m_run && isRunning())
  {
    EventJoystick eventJoy;

    ssize_t n = read (m_fd, &joy_event, sizeof(struct js_event));
    if (!n)
      std::cerr << "No joy_event could be read\n";

    eventJoy.time = joy_event.time;
    eventJoy.value = joy_event.value;

    switch (joy_event.type)
    {
    case JS_EVENT_BUTTON:
      if (!m_init) m_init = true;
      eventJoy.number = joy_event.number;
      eventJoy.synthetic = false;
    {
        QMutexLocker locker(&mBtnMutex);
        mEventsBtn.push(eventJoy);
        (void)locker;
    }
      //signalButton.emit (eventJoy);
      break;

    case JS_EVENT_AXIS:
      if (!m_init) m_init = true;
      eventJoy.number = joy_event.number;
      eventJoy.synthetic = false;

    {
        QMutexLocker locker(&mAxisMutex);
        mEventsAxis.push(eventJoy);
        (void)locker;
    }
      //signalAxis.emit (eventJoy);
      break;

    case JS_EVENT_BUTTON | JS_EVENT_INIT:
      if (m_init) // skip the synthetic events on driver start
      {
        eventJoy.number = joy_event.number & ~JS_EVENT_INIT;
        eventJoy.synthetic = true;

        {
            QMutexLocker locker(&mBtnMutex);
            mEventsBtn.push(eventJoy);
            (void)locker;
        }
          //signalButton.emit (eventJoy);
      }
      break;

    case JS_EVENT_AXIS | JS_EVENT_INIT:
      if (m_init) // skip the synthetic events on driver start
      {
        eventJoy.number = joy_event.number & ~JS_EVENT_INIT;
        eventJoy.synthetic = true;
        {
            QMutexLocker locker(&mAxisMutex);
            mEventsAxis.push(eventJoy);
            (void)locker;
        }
          //signalAxis.emit (eventJoy);
      }
      break;

    default: // we should never reach this point
      printf ("unknown event: %x\n", joy_event.type);
    }
  }
}

int Joystick::getNumberOfButtons ()
{
  return m_buttons;
}

int Joystick::getNumberOfAxes ()
{
  return m_axes;
}

const string &Joystick::getIdentifier ()
{
    return m_name;
}

bool Joystick::pollEventButton(EventJoystick &_event)
{
    if ( mEventsBtn.size() == 0 ) {
        return false;
    }

    QMutexLocker locker(&mBtnMutex);
    (void)locker;

    if ( mEventsBtn.size() > 0 )
    {
        EventJoystick ev = mEventsBtn.front();
        mEventsBtn.pop();
        _event = ev;
        return true;
    }

    return false;
}
bool Joystick::pollEventAxis(EventJoystick &_event)
{
    if ( mEventsAxis.size() == 0 ) return false;

    QMutexLocker locker(&mAxisMutex);
    (void)locker;

    if ( mEventsAxis.size() > 0 )
    {
        EventJoystick ev = mEventsAxis.front();
        mEventsAxis.pop();

        _event = ev;
        return true;
    }

    return false;
}

#endif // ACGL_OWN_LINUX_JOYSTICK
