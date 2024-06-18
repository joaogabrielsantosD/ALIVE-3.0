#ifndef TICKERISR_H
#define TICKERISR_H

#include <Ticker.h>
#include "AcquisitionData.h"

void init_tickers(void);
bool checkPID(void);

/* ISRs */
void ticker100mHzISR(void);
void ticker200mHzISR(void);
void ticker300mHzISR(void);
void ticker1HzISR(void);
void ticker2HzISR(void);

#endif