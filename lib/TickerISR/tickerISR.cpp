#include "tickerISR.h"

TickerISRHandler TickerISR;

void TickerISRHandler::init_tickers()
{
  this->tickerONCE.once(1.0f, PIDs_once);           // one time
  this->ticker5min.attach(300.0f, ticker_5min_ISR); // 300s == 5min
  this->ticker1min.attach(60.0f, ticker_1min_ISR);
  this->ticker30secs.attach(30.0f, ticker_30sec_ISR);
  this->ticker5secs.attach(5.0f, ticker_5sec_ISR);
  this->ticker1secs.attach(1.0f, ticker_1sec_ISR);
}

/*=========================== ISRs ====================================*/

void TickerISRHandler::PIDs_once()
{
  for (int i = MonitorStatus; i <= DPF_Temperature; i++)
    CircularBufferState.insert(i, false);
  
  CircularBufferState.insert(Odometer_PID, false);
  CircularBufferState.insert(DTC_mode_3, false);
}

void TickerISRHandler::ticker_5min_ISR()
{
  CircularBufferState.insert(DistanceTraveledSinceCodeCleared);
  CircularBufferState.insert(DistanceTraveledMIL);
  CircularBufferState.insert(Odometer_PID);
  CircularBufferState.insert(EthanolFuel);
  CircularBufferState.insert(FuelLevelInput);
}

void TickerISRHandler::ticker_1min_ISR()
{
  CircularBufferState.insert(ControlModuleVoltage);
  CircularBufferState.insert(FuelPressure);
  CircularBufferState.insert(AbsoluteFuelRailPressure);

  CircularBufferState.insert(TimeRun_MIL);
  CircularBufferState.insert(TimeSinceTroubleCodesCleared);
  // CircularBufferState.insert(CommandedEGR_ERROR);
  CircularBufferState.insert(EngineRunTime);

  CircularBufferState.insert(ShortTermFuel_Bank1);
  CircularBufferState.insert(LongTermFuel_Bank1);
  CircularBufferState.insert(ShortTermFuel_Bank2);
  CircularBufferState.insert(LongTermFuel_Bank2);
}

void TickerISRHandler::ticker_30sec_ISR()
{
  // CircularBufferState.insert(GPS_ST);
  CircularBufferState.insert(EngineCollantTemp);
  CircularBufferState.insert(IntakeAirTemperature);

  CircularBufferState.insert(CatalystTemperature_Bank1Sensor1);
  CircularBufferState.insert(CatalystTemperature_Bank2Sensor1);
  CircularBufferState.insert(CatalystTemperature_Bank1Sensor2);
  CircularBufferState.insert(CatalystTemperature_Bank2Sensor2);

  CircularBufferState.insert(BarometricPressure);
  CircularBufferState.insert(AmbientAirTemperature);

  CircularBufferState.insert(AbsoluteVapourPressure);
  CircularBufferState.insert(EvapSystemVaporPressure);
  CircularBufferState.insert(EngineOilTemperature);

  CircularBufferState.insert(EngineCoolantTemperature);
  CircularBufferState.insert(IntakeAirTemperatureSensor);
  // CircularBufferState.insert(ExhaustGasRecircuilationTemperature);

  // CircularBufferState.insert(TurboChargerCompressorPressure);
  // CircularBufferState.insert(BoostPressureControl);
  // CircularBufferState.insert(VGT);
  // CircularBufferState.insert(WastegateControl);
  // CircularBufferState.insert(ExhaustPressure);

  // CircularBufferState.insert(TurbochargerTemperature1);
  // CircularBufferState.insert(TurbochargerTemperature2);
  // CircularBufferState.insert(ChargeAIR_CACT);
  // CircularBufferState.insert(EGT_Bank1);
  // CircularBufferState.insert(EGT_Bank2);
  CircularBufferState.insert(DPF_Temperature);
  // CircularBufferState.insert(NOxNTE);
  // CircularBufferState.insert(PMxNTE);

  // CircularBufferState.insert(FuelPressureControlSystem);
  
  // CircularBufferState.insert(InjectionPressureControl);
  // CircularBufferState.insert(DPF1);
  // CircularBufferState.insert(DPF2);
}

void TickerISRHandler::ticker_5sec_ISR()
{
  CircularBufferState.insert(O2S1_WR_lambda2, false);
  CircularBufferState.insert(O2S2_WR_lambda2, false);
  CircularBufferState.insert(O2S3_WR_lambda2, false);
  CircularBufferState.insert(O2S4_WR_lambda2, false);
  CircularBufferState.insert(O2S5_WR_lambda2, false);
  CircularBufferState.insert(O2S6_WR_lambda2, false);
  CircularBufferState.insert(O2S7_WR_lambda2, false);
  CircularBufferState.insert(O2S8_WR_lambda2, false);

  // CircularBufferState.insert(OxygenSensorsPresent);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3, false);
  CircularBufferState.insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4, false);

  CircularBufferState.insert(CommandEquivalenceRatio, false);

  CircularBufferState.insert(ShortTermSecondaryOxygenSensor_bank1bank3, false);
  CircularBufferState.insert(LongTermSecondaryOxygenSensor_bank1bank3, false);
  CircularBufferState.insert(ShortTermSecondaryOxygenSensor_bank2bank4, false);
  CircularBufferState.insert(LongTermSecondaryOxygenSensor_bank2bank4, false);

  CircularBufferState.insert(FuelRailPressure_vac, false);
  CircularBufferState.insert(FuelRailPressure_dis, false);
}

void TickerISRHandler::ticker_1sec_ISR()
{
  // CircularBufferState.insert(Accelerometer_ST);
  CircularBufferState.insert(EngineRPM, false);
  CircularBufferState.insert(VehicleSpeed, false);

  CircularBufferState.insert(TimingAdvance, false);
  CircularBufferState.insert(ThrottlePosition, false);
  CircularBufferState.insert(RelativeThrottlePosition, false);
  CircularBufferState.insert(AbsoluteThrottlePositionB, false);
  CircularBufferState.insert(AbsoluteThrottlePositionC, false);
  CircularBufferState.insert(AcceleratorPedalPositionD, false);
  CircularBufferState.insert(AcceleratorPedalPositionE, false);
  CircularBufferState.insert(AcceleratorPedalPositionF, false);
  CircularBufferState.insert(CommandedThrottleActuator, false);
  CircularBufferState.insert(RelativeAcceleratorPedalPosition, false);
  // CircularBufferState.insert(CommandedThrottleActuator2Position);

  CircularBufferState.insert(EngineLoad, false);
  CircularBufferState.insert(IntakeManifoldAbsolutePressure, false);
  CircularBufferState.insert(MAFairFlowRate, false);

  CircularBufferState.insert(VaporPressure, false);
  CircularBufferState.insert(MaximumValueForEquivalenceRatio, false);
  CircularBufferState.insert(MaximumValueForAirFlowRate, false);
  CircularBufferState.insert(DriverDemandEngine, false);
  CircularBufferState.insert(ActualEngine_PercentTorque, false);
  CircularBufferState.insert(EngineReferenceTorque, false);
  CircularBufferState.insert(EnginePercentTorque, false);

  CircularBufferState.insert(MassAirFlowSensor, false);
  // CircularBufferState.insert(CommandedDiesel);
  // CircularBufferState.insert(TurbochargerRPM);
  CircularBufferState.insert(RunTimeSinceEngineStart, false);

  CircularBufferState.insert(AbsoluteLoadValue, false);
}
