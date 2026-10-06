/**
 * \file
 * Timer library implementation for Arduino IDE.
 *
 * Based on the uIP timer library by Adam Dunkels.
 */

#include "timer.h"

#define TIMER_ACTIVE_FLAG 0x1234

/*---------------------------------------------------------------------------*/
/**
 * Set a timer.
 *
 * This function is used to set a timer for a time sometime in the
 * future. The function timer_expired() will evaluate to true after
 * the timer has expired.
 *
 * \param t A pointer to the timer
 * \param interval The interval before the timer expires.
 */
void
timer_set(struct timer *t, clock_time_t interval)
{
  t->interval = interval;
  t->active   = TIMER_ACTIVE_FLAG;
  t->start    = millis();
}
/*---------------------------------------------------------------------------*/
/**
 * Reset the timer with the same interval.
 *
 * This function resets the timer with the same interval that was
 * given to the timer_set() function. The start point of the interval
 * is the exact time that the timer last expired. Therefore, this
 * function will cause the timer to be stable over time, unlike the
 * timer_restart() function.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_restart()
 */
void
timer_reset(struct timer *t)
{
  t->active = TIMER_ACTIVE_FLAG;
  t->start += t->interval;
}
/*---------------------------------------------------------------------------*/
/**
 * Restart the timer from the current point in time.
 *
 * This function restarts a timer with the same interval that was
 * given to the timer_set() function. The timer will start at the
 * current time.
 *
 * \note A periodic timer will drift if this function is used to reset
 * it. For periodic timers, use the timer_reset() function instead.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_reset()
 */
void
timer_restart(struct timer *t)
{
  t->active = TIMER_ACTIVE_FLAG;
  t->start  = millis();
}
/*---------------------------------------------------------------------------*/
/**
 * Check if a timer has expired.
 *
 * This function tests if a timer has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer
 *
 * \return Non-zero if the timer has expired, zero otherwise.
 */
int
timer_expired(struct timer *t)
{
  if (t->active == TIMER_ACTIVE_FLAG)
    return (clock_time_t)(millis() - t->start) >= (clock_time_t)t->interval;

  return 0;
}
/*---------------------------------------------------------------------------*/
int
timer_active(struct timer *t)
{
  return (t->active == TIMER_ACTIVE_FLAG) ? 1 : 0;
}
/*---------------------------------------------------------------------------*/
void
timer_stop(struct timer *t)
{
  t->active = 0;
}
/*---------------------------------------------------------------------------*/
void
Delay(clock_time_t msDelay)
{
  delay((unsigned long)msDelay);
}
/*---------------------------------------------------------------------------*/
/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in milliseconds.
 */
clock_time_t
clock_time(void)
{
  return (clock_time_t)millis();
}

/** @} */
