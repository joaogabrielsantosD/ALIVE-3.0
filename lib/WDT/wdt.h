#ifndef WDT_H
#define WDT_H

#include <esp_task_wdt.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/rtc_wdt.h>

void set_wdt_timer(void);
void reset_rtc_wdt(void);

#endif