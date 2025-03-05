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
    insert(i, false);
  
  insert(Odometer_PID, false);
  insert(DTC_mode_3, false);
}

void TickerISRHandler::ticker_5min_ISR()
{
  insert(DistanceTraveledSinceCodeCleared);
  insert(DistanceTraveledMIL);
  insert(Odometer_PID);
  insert(EthanolFuel);
  insert(FuelLevelInput);
}

void TickerISRHandler::ticker_1min_ISR()
{
  insert(ControlModuleVoltage);
  insert(FuelPressure);
  insert(AbsoluteFuelRailPressure);

  insert(TimeRun_MIL);
  insert(TimeSinceTroubleCodesCleared);
  // insert(CommandedEGR_ERROR);
  insert(EngineRunTime);

  insert(ShortTermFuel_Bank1);
  insert(LongTermFuel_Bank1);
  insert(ShortTermFuel_Bank2);
  insert(LongTermFuel_Bank2);
}

void TickerISRHandler::ticker_30sec_ISR()
{
  // insert(GPS_ST);
  insert(EngineCollantTemp);
  insert(IntakeAirTemperature);

  insert(CatalystTemperature_Bank1Sensor1);
  insert(CatalystTemperature_Bank2Sensor1);
  insert(CatalystTemperature_Bank1Sensor2);
  insert(CatalystTemperature_Bank2Sensor2);

  insert(BarometricPressure);
  insert(AmbientAirTemperature);

  insert(AbsoluteVapourPressure);
  insert(EvapSystemVaporPressure);
  insert(EngineOilTemperature);

  insert(EngineCoolantTemperature);
  insert(IntakeAirTemperatureSensor);
  // insert(ExhaustGasRecircuilationTemperature);

  // insert(TurboChargerCompressorPressure);
  // insert(BoostPressureControl);
  // insert(VGT);
  // insert(WastegateControl);
  // insert(ExhaustPressure);

  // insert(TurbochargerTemperature1);
  // insert(TurbochargerTemperature2);
  // insert(ChargeAIR_CACT);
  // insert(EGT_Bank1);
  // insert(EGT_Bank2);
  insert(DPF_Temperature);
  // insert(NOxNTE);
  // insert(PMxNTE);

  // insert(FuelPressureControlSystem);
  
  // insert(InjectionPressureControl);
  // insert(DPF1);
  // insert(DPF2);
}

void TickerISRHandler::ticker_5sec_ISR()
{
  insert(O2S1_WR_lambda2, false);
  insert(O2S2_WR_lambda2, false);
  insert(O2S3_WR_lambda2, false);
  insert(O2S4_WR_lambda2, false);
  insert(O2S5_WR_lambda2, false);
  insert(O2S6_WR_lambda2, false);
  insert(O2S7_WR_lambda2, false);
  insert(O2S8_WR_lambda2, false);

  // insert(OxygenSensorsPresent);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3, false);
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4, false);

  insert(CommandEquivalenceRatio, false);

  insert(ShortTermSecondaryOxygenSensor_bank1bank3, false);
  insert(LongTermSecondaryOxygenSensor_bank1bank3, false);
  insert(ShortTermSecondaryOxygenSensor_bank2bank4, false);
  insert(LongTermSecondaryOxygenSensor_bank2bank4, false);

  insert(FuelRailPressure_vac, false);
  insert(FuelRailPressure_dis, false);
}

void TickerISRHandler::ticker_1sec_ISR()
{
  // insert(Accelerometer_ST);
  insert(EngineRPM, false);
  insert(VehicleSpeed, false);

  insert(TimingAdvance, false);
  insert(ThrottlePosition, false);
  insert(RelativeThrottlePosition, false);
  insert(AbsoluteThrottlePositionB, false);
  insert(AbsoluteThrottlePositionC, false);
  insert(AcceleratorPedalPositionD, false);
  insert(AcceleratorPedalPositionE, false);
  insert(AcceleratorPedalPositionF, false);
  insert(CommandedThrottleActuator, false);
  insert(RelativeAcceleratorPedalPosition, false);
  // insert(CommandedThrottleActuator2Position);

  insert(EngineLoad, false);
  insert(IntakeManifoldAbsolutePressure, false);
  insert(MAFairFlowRate, false);

  insert(VaporPressure, false);
  insert(MaximumValueForEquivalenceRatio, false);
  insert(MaximumValueForAirFlowRate, false);
  insert(DriverDemandEngine, false);
  insert(ActualEngine_PercentTorque, false);
  insert(EngineReferenceTorque, false);
  insert(EnginePercentTorque, false);

  insert(MassAirFlowSensor, false);
  // insert(CommandedDiesel);
  // insert(TurbochargerRPM);
  insert(RunTimeSinceEngineStart, false);

  insert(AbsoluteLoadValue, false);
}
