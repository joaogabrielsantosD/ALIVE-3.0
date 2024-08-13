#ifndef TICKERISR_H
#define TICKERISR_H

#include <Ticker.h>
#include "AcquisitionData.h"

void init_tickers(void);
bool checkPID(void);
void Call_DTC_mode3(void);

/* ISRs */
void PIDs_once(void);       // Only 1 time
void ticker_5min_ISR(void); // 5 minutes == 300 seconds
void ticker_1min_ISR(void);
void ticker_30sec_ISR(void);
void ticker_10sec_ISR(void);
void ticker_5sec_ISR(void);
void ticker_1sec_ISR(void);
void ticker_05sec_ISR(void);
void ticker_01sec_ISR(void);

#endif