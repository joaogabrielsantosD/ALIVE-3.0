#ifndef TICKERISR_H
#define TICKERISR_H

#pragma once

#include <Ticker.h>
#include "CircularBufferState.h"

class TickerISRHandler
{
private:
    Ticker tickerONCE;
    Ticker ticker5min;
    Ticker ticker1min;
    Ticker ticker30secs;
    Ticker ticker5secs;
    Ticker ticker1secs;

public:
    TickerISRHandler() = default; 

    /**
    * @brief Initialize all tickers to insert the messages in the circular buffer
    * @note Initializes multiple timers with different intervals, each triggering specific ISR functions.
    */
    void init_tickers(void);

private: /* ISRs */
    /**
    * @brief Insert all PIDs in the circular buffer
    * @note Inserts OBD-II messages into a circular buffer for data acquisition, 
    * covering various PIDs from MonitorStatus to DPF_Temperature, as well as Odometer_PID and DTC_mode_3.
    */
    static void PIDs_once(void);

    /**
    * @brief Insert PIDs in `5 minutes (300 seconds)` in the circular buffer
    * @note Inserts OBD-II messages related to distance traveled, fuel, 
    * and odometer data into a buffer every 5 minutes.
    */
    static void ticker_5min_ISR(void);
    
    /**
    * @brief Insert PIDs in `1 minutes (60 seconds)` in the circular buffer
    * @note Inserts OBD-II messages related to fuel pressure, engine runtime, 
    * fuel trim data, and other parameters into a buffer every 1 minute.
    */
    static void ticker_1min_ISR(void);
    
    /**
    * @brief Insert PIDs in `30 seconds` in the circular buffer
    * @note Inserts various OBD-II parameters, including engine temperatures, 
    * catalyst and barometric pressures, and other vehicle data into a buffer every 30 seconds.
    */    
    static void ticker_30sec_ISR(void);
    
    /**
    * @brief Insert PIDs in `5 seconds` in the circular buffer
    * @note Inserts various OBD-II oxygen sensor data, fuel trim values, 
    * and fuel rail pressures into a buffer every 5 seconds.
    */    
    static void ticker_5sec_ISR(void);
    
    /**
    * @brief Insert PIDs in `1 second` in the circular buffer
    * @note Inserts various engine and vehicle performance parameters, such as RPM, speed, 
    * throttle position, and air flow rates into a buffer every 1 second.
    */    
    static void ticker_1sec_ISR(void);
};

extern TickerISRHandler TickerISR;

#endif // TICKERISR_H
