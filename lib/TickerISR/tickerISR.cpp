#include "tickerISR.h"

Ticker ticker1Hz;
Ticker ticker10Hz;
Ticker ticker20Hz;

void setup_ticker()
{
  ticker1Hz.attach(1.0, PIDs_1hz);
  ticker10Hz.attach(2.0, PIDs_10hz);
  ticker20Hz.attach(20.0, PIDs_20hz);
}

void PIDs_1hz()
{
  insert(DistanceTraveled_ST);
  insert(EngineRPM_ST);
}

void PIDs_10hz()
{
  insert(VehicleSpeed_ST);
  insert(FuelLevel_ST);
  insert(EngineCoolant_ST);  
}

void PIDs_20hz()
{

}