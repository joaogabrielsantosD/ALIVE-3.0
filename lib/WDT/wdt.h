#ifndef WDT_H
#define WDT_H

#pragma once

#include <esp_task_wdt.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/rtc_wdt.h>

class WDTHandler
{
public:
    WDTHandler() = default;

    /*
    * @brief Config and Init the WDT
    * @note Configures the RTC Watchdog Timer (WDT) to reset the system after 9000ms.
    * @reference https://www.youtube.com/watch?v=yOwg1EMby2g&t=240s
    */
    void set_wdt_timer(void);

    /*
    * @brief Reset the WDT
    * @note Resets both the ESP task watchdog and the RTC Watchdog Timer (WDT).
    */
    void reset_rtc_wdt(void);
};

extern WDTHandler WDT;

#endif // WDT_H
