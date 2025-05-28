#include "screen_sleep.h"
#include "cmsis_os.h"
#include "lcd.h"

#define TIMEOUT 100

TimerHandle_t sleep_timer;

void
vTimerCallback (TimerHandle_t xTimer)
{
    LCD_BL (0);
}

void
screen_sleep_init (void)
{
    sleep_timer = xTimerCreate (/* Just a text name, not used by the RTOS
                                kernel. */
                                "screen_sleep",
                                /* The timer period in ticks, must be
                                greater than 0. */
                                TIMEOUT * 1000,
                                /* The timers will auto-reload themselves
                                when they expire. */
                                pdFALSE,
                                /* The ID is used to store a count of the
                                number of times the timer has expired, which
                                is initialised to 0. */
                                (void *)0,
                                /* Each timer calls the same callback when
                                it expires. */
                                vTimerCallback);
    xTimerStart (sleep_timer, 0);
}