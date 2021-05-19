#ifndef Servo_h
#define Servo_h

#include <inttypes.h>

#define MIN_PULSE_WIDTH 544      // the shortest pulse sent to a MWservo
#define MAX_PULSE_WIDTH 2400     // the longest pulse sent to a MWservo
#define DEFAULT_PULSE_WIDTH 1500 // default pulse width when MWservo is attached
#define REFRESH_INTERVAL 20000   // minumim time to refresh servos in microseconds

class MWservo
{
public:
  MWservo();
  void init(int pin);     // initailizes the servo
  void moveTo(int value); // angle
};

#endif
