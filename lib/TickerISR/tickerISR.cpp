#include "tickerISR.h"

Ticker tickerONCE, ticker5min, ticker1min, ticker30secs, ticker10secs, 
    ticker5secs, ticker1secs, ticker_05sec, ticker_01sec;

/* Initialize all tickers to insert the messages in the circular buffer */
void init_tickers()
{
  //tickerONCE.once(1.0f, PIDs_once);           // one time
  //ticker5min.attach(300.0f, ticker_5min_ISR); // 300s == 5min  
  //ticker1min.attach(60.0f, ticker_1min_ISR);
  //ticker30secs.attach(30.0f, ticker_30sec_ISR);
  //ticker10secs.attach(10.0f, ticker_10sec_ISR);
  //ticker5secs.attach(5.0f, ticker_5sec_ISR);
  //ticker1secs.attach(1.0f, ticker_1sec_ISR);
  ticker_05sec.attach(0.5f, ticker_05sec_ISR);
  //ticker_01sec.attach(0.1f, ticker_01sec_ISR);
}

void Call_DTC_mode3(void)
{
  #ifdef Print_in_serial
    if (insert(DTC_mode_3))
      Serial.println("DTC enviado com sucesso");
    else
      Serial.println("Erro ao enviar o DTC");
  #else
    insert(DTC_mode_3);
  #endif
}

/*=========================== ISRs ====================================*/
void PIDs_once()
{
  insert(FuelType);
  insert(HybridBatteryLife);  
}

void ticker_5min_ISR()
{
  insert(DistanceTraveledSinceCodeCleared);
  insert(DistanceTraveledMIL); 
  insert(Odometer_PID);
  insert(EthanolFuel);
}

void ticker_1min_ISR()
{
  insert(FuelPressure);  

  insert(TimeRun_MIL);
  insert(TimeSinceTroubleCodesCleared);
  //insert(CommandedEGR_ERROR);
  insert(EngineRunTime);

  insert(ShortTermFuel_Bank1);                                       
  insert(LongTermFuel_Bank1);                                        
  insert(ShortTermFuel_Bank2);                                       
  insert(LongTermFuel_Bank2);
}

void ticker_30sec_ISR()
{
  //insert(GPS_ST);
  insert(ControlModuleVoltage);
  insert(AbsoluteLoadValue);
  insert(AbsoluteFuelRailPressure);
  //insert(FuelPressureControlSystem);
  //insert(FuelLevelInput);
  //insert(InjectionPressureControl);
  //insert(DPF1);
  //insert(DPF2);
}

void ticker_10sec_ISR()
{
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
  //insert(ExhaustGasRecircuilationTemperature);

  //insert(TurboChargerCompressorPressure);
  //insert(BoostPressureControl);
  //insert(VGT);
  //insert(WastegateControl);
  //insert(ExhaustPressure);

  //insert(TurbochargerTemperature1);
  //insert(TurbochargerTemperature2);
  //insert(ChargeAIR_CACT);
  //insert(EGT_Bank1);
  //insert(EGT_Bank2);
  insert(DPF_Temperature);
  //insert(NOxNTE);
  //insert(PMxNTE);
}

void ticker_5sec_ISR()
{
  insert(O2S1_WR_lambda2);
  insert(O2S2_WR_lambda2);
  insert(O2S3_WR_lambda2);
  insert(O2S4_WR_lambda2);
  insert(O2S5_WR_lambda2);
  insert(O2S6_WR_lambda2);
  insert(O2S7_WR_lambda2);
  insert(O2S8_WR_lambda2);

  //insert(OxygenSensorsPresent);                              
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1);                           
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2);                                                  
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3);                                              
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4);                                              
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1);                                              
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2);                                              
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3);                                              
  insert(OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4);    

  insert(CommandEquivalenceRatio);

  insert(ShortTermSecondaryOxygenSensor_bank1bank3);
  insert(LongTermSecondaryOxygenSensor_bank1bank3);
  insert(ShortTermSecondaryOxygenSensor_bank2bank4);
  insert(LongTermSecondaryOxygenSensor_bank2bank4);

  insert(FuelRailPressure_vac);                                                 
  insert(FuelRailPressure_dis); 
}

void ticker_1sec_ISR()
{
  insert(EngineLoad);
  insert(IntakeManifoldAbsolutePressure);
  insert(MAFairFlowRate);  

  insert(VaporPressure);
  insert(MaximumValueForEquivalenceRatio);
  insert(MaximumValueForAirFlowRate);
  insert(DriverDemandEngine);
  insert(ActualEngine_PercentTorque);
  insert(EngineReferenceTorque);
  insert(EnginePercentTorque);

  insert(MassAirFlowSensor);
  //insert(CommandedDiesel);
  //insert(TurbochargerRPM);
  insert(RunTimeSinceEngineStart);
}

void ticker_05sec_ISR()
{ 
  //insert(Accelerometer_ST); 
  insert(EngineRPM);
  insert(VehicleSpeed);
}

void ticker_01sec_ISR()
{
  insert(TimingAdvance);
  insert(ThrottlePosition);
  insert(RelativeThrottlePosition);
  insert(AbsoluteThrottlePositionB);
  insert(AbsoluteThrottlePositionC); 
  insert(AcceleratorPedalPositionD);
  insert(AcceleratorPedalPositionE);
  insert(AcceleratorPedalPositionF);
  insert(CommandedThrottleActuator);
  insert(RelativeAcceleratorPedalPosition);
  //insert(CommandedThrottleActuator2Position);
}
