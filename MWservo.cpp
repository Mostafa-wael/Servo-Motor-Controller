/**
 * @author Mostafa Wael
 * @details 
 * solo servo motor library for AVR 
 * this library is the most optimized libraty for dealing with single serovo motor, reducing the overhead of using multiple servos and timers
 */
#include <avr/interrupt.h>
#include <Arduino.h>
#include "MWservo.h"

#define usToangle(_us) ((clockCyclesPerMicrosecond() * _us) / 8) // converts microseconds to tick (assumes prescale of 8)
#define TRIM_DURATION 2 // compensation angle to trim adjust for digitalWrite delays // 12
#define ServoCount 1    // the total number of attached servos

static unsigned int angle;
static unsigned int SERVO_PIN;
static int8_t channel; // counter for the servo being pulsed (or -1 if refresh interval)

/**
 * @brief the interrupt handler of the timer
 * 
 * @param TCNTn  It is a timer/counter register. This is main register where the counts of the timer are loaded. 
 * So assuming the prescaler to be 1, the pos in this register will increment at every clock cycle.
 * @param OCRnA  his is an output compare register. 
 * The pos in the TCNT can be compared with the pos in this register and program the controller to raise an interrupt when the pos in TCNT matches the values in OCRn. 
 * So, OCR determines how much time before the interrupt is raised.
 */
static inline void interruptHandler(volatile uint16_t *TCNTn, volatile uint16_t *OCRnA)
{
  if (channel < 0)
    *TCNTn = 0; // channel set to -1 indicated that refresh interval completed so reset the timer
  else
    digitalWrite(SERVO_PIN, LOW); // pulse this channel low if activated

  channel++; // increment to the next channel
  if (channel < ServoCount)
  {
    *OCRnA = *TCNTn + angle;
    digitalWrite(SERVO_PIN, HIGH); // its an active channel so pulse it high
  }
  else
  {
    // finished all channels so wait for the refresh period to expire before starting over
    if (((unsigned)*TCNTn) + 4 < usToangle(REFRESH_INTERVAL)) // allow a few angle to ensure the next OCR1A not missed
      *OCRnA = (unsigned int)usToangle(REFRESH_INTERVAL);
    else
      *OCRnA = *TCNTn + 4; // at least REFRESH_INTERVAL has elapsed
    channel = -1;          // this will get incremented at the end of the refresh period to start again at the first channel
  }
}
/**
 * @brief setting the interrupt handler
 * 
 */
// Interrupt handlers for Arduino
SIGNAL(TIMER1_COMPA_vect)
{
  interruptHandler(&TCNT1, &OCR1A);
}
/**
 * @brief initialize the interrupt service routine
 * 
 */
static void initISR()
{
  TCCR1A = 0;         // normal counting mode
  TCCR1B = _BV(CS11); // set prescaler of 8
  TCNT1 = 0;          // clear the timer count
  // here if not ATmega8 or ATmega128
  TIFR1 |= _BV(OCF1A);   // clear any pending interrupts;
  TIMSK1 |= _BV(OCIE1A); // enable the output compare interrupt
}

/**
 * @brief Constructor
 * 
 */
MWservo::MWservo()
{
  angle = usToangle(DEFAULT_PULSE_WIDTH); // store default values
}

/**
 * @brief initialize the servo motor and setting its pin
 * 
 * @param pin servo motor pin
 */
void MWservo::init(int pin)
{
  SERVO_PIN = pin;
  pinMode(SERVO_PIN, OUTPUT); // set MWservo SERVO_PIN to output
  // initialize the timer if it has not already been initialized
  initISR();
}
/**
 * @brief move the motor to this position
 * 
 * @param pos 
 */
void MWservo::moveTo(int pos)
{
  if (pos < MIN_PULSE_WIDTH)
  { // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
    pos = map(pos, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  }

  pos -= TRIM_DURATION;
  pos = usToangle(pos); // convert to angle after compensating for interrupt overhead

  uint8_t oldSREG = SREG;
  cli();
  angle = pos;
  SREG = oldSREG;
}

