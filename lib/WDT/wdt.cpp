#include "wdt.h"

WDTHandler WDT;

void WDTHandler::set_wdt_timer()
{
  rtc_wdt_protect_off(); // Disable RTC WDT write protection
  rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_RTC);
  // Set stage 0 to trigger a system reset after 9000ms
  rtc_wdt_set_time(RTC_WDT_STAGE0, 9000);
  rtc_wdt_enable();
  rtc_wdt_protect_on();
}

void WDTHandler::reset_rtc_wdt()
{
  esp_task_wdt_reset(); // Reset the ESP task watchdog timer
  rtc_wdt_feed();       // Feed the RTC WDT to reset its timer
}
