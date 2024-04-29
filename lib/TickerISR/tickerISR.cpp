#include "tickerISR.h"

bool Print_in_serial = false; // Put True or False to enable SerialPrint of checkPID
Ticker ticker1Hz,
       ticker10Hz,  // atualizar inserts
       ticker20Hz;

void setup_ticker()
{
  do 
  {
    Serial.println("Check the PID support...");
  } while(checkPID());
  
  ticker1Hz.attach(1.0, PIDs_1hz);
  //ticker10Hz.attach(2.0, PIDs_10hz);
  //ticker20Hz.attach(3.0, PIDs_20hz);
}

bool checkPID()
{
  // Flag to check if you received the PID support message
  bool check_receive_pid = false;
  unsigned char Data[8] = {0x04, 0x01, 0x00/*=ID*/, 0x00, 0x00, 0x00, 0x00, 0x00};

  for(int i = 1; i < 5; i++)
  {
    Serial.printf("Trying to send PID%d support, please turn on the car electronics\r\n", i);
    check_receive_pid = false;

    while(!check_receive_pid)
    {
      if(i==1)
      {
        Data[2] = PIDsupported1;
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_CANroutine();
        check_receive_pid = true;
      }

      if(i==2)
      {
        Data[2] = PIDsupported2;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_CANroutine();
        check_receive_pid = true;
      }

      if(i==3)
      {
        Data[2] = PIDsupported3;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_CANroutine();
        check_receive_pid = true;
      }

      if(i==4)
      {
        Data[2] = PIDsupported4;
        //if(send_msg(Data) && Print_in_serial) debug_print(Data);
        while(!checkReceive())
        {
          if(send_msg(Data) && Print_in_serial) debug_print(Data);
          vTaskDelay(100);          
        }
        MsgRec_CANroutine();
        check_receive_pid = true;
      }

      vTaskDelay(1);
    }
     
    if((i+1)==5) break;
  }

  return false;
}

void PIDs_1hz()
{
  /*===== Id for bit encode ====== */
    //insert(MonitorStatus);
    //insert(OxygenSensorsPresent);
    //insert(OBDstandard);
    //insert(MonitorStatusDriveCycle);
    //insert(EmissionRequirements);
  /*====== Id for test ============*/
  /* ===== PID RESQUEST 2  ===== */
  // B8 = 10111000
    insert(DistanceTraveledSinceCodeCleared);
    insert(VaporPressure);
    insert(BarometricPressure);
    insert(O2S1_WR_lambda2);
    insert(O2S2_WR_lambda2);
    insert(O2S3_WR_lambda2);
    insert(O2S4_WR_lambda2);
    insert(O2S5_WR_lambda2);
  // 01 = 0000000 /1
    //insert(O2S6_WR_lambda2);
    //insert(O2S7_WR_lambda2);
    //insert(O2S8_WR_lambda2);
    //insert(CatalystTemperature_Bank1Sensor1);
    //insert(CatalystTemperature_Bank2Sensor1);
    //insert(CatalystTemperature_Bank1Sensor2);
    //insert(CatalystTemperature_Bank2Sensor2);
  /* ===== PID RESQUEST 3  ===== */
  // 48 = 0/ 1001000
    //insert(ControlModuleVoltage);
    //insert(AbsoluteLoadValue);
    //insert(CommandEquivalenceRatio);
    //insert(RelativeThrottlePosition);
    //insert(AmbientAirTemperature);
    //insert(AbsoluteThrottlePositionB);
    //insert(AbsoluteThrottlePositionC);    
  // D2 = 11010010
    //insert(AcceleratorPedalD);
    //insert(AcceleratorPedalE);
    //insert(AcceleratorPedalF);
    //insert(CommandedThrottleActuator);
    //insert(TimeRun_MIL);
    //insert(TimeSinceTroubleCodesCleared);
    //insert(MaximumValueForEquivalenceRatio);
    //insert(MaximumValueForAirFlowRate);
  // 00 = 00000000
    //insert(FuelType);
    //insert(EthanolFuel);
    //insert(AbsoluteVapourPressure);
    //insert(EvapSystemVaporPressure);
    //insert(ShortTermSecondaryOxygenSensor_bank1bank3);
    //insert(LongTermSecondaryOxygenSensor_bank1bank3);
    //insert(ShortTermSecondaryOxygenSensor_bank2bank4);
    //insert(LongTermSecondaryOxygenSensor_bank2bank4);
  // 01 = 0000000 /1
    //insert(AbsoluteFuelRailPressure);
    //insert(RelativeAcceleratorPedalPosition);
    //insert(HybridBatteryLife);
    //insert(EngineOilTemperature);
    //insert(FuelInjectionTiming);
    //insert(EngineFuelRate);
  /* ===== PID RESQUEST 4  ===== */
  // 00 = 00000000
    //insert(DriverDemandEngine);
    //insert(ActualEngine_PercentTorque);
    //insert(EngineReferenceTorque);
    //insert(EnginePercentTorque);
    //insert(AuxiliaryInput);
    //insert(MassAirFlowSensor);
    //insert(EngineCoolantTemperature);
    //insert(IntakeAirTemperatureSensor);    
  // 00 = 00000000 
    //insert(CommandedEGR_ERROR);
    //insert(CommandedDiesel);
    //insert(ExhaustGas);
    //insert(CommandedThrottleActuator2);
    //insert(FuelPressureControlSystem);
    //insert(InjectionPressureControl);
    //insert(TurboChargerCompressor);
    //insert(BoostPressureControl);
  // 0A = 00001010
    //insert(VGT);
    //insert(WastegateControl);
    //insert(ExhaustPressure);
    //insert(TurbochargerRPM);
    //insert(TurbochargerTemperature1);
    //insert(TurbochargerTemperature2);
    //insert(ChargeAIR_CACT);
    //insert(EGT_Bank1);
  // 10 = 00010000
    //insert(EGT_Bank2);
    //insert(DPF1);
    //insert(DPF2);
    //insert(DPF_Temperature);
    //insert(NOxNTE);
    //insert(PMxNTE);
    //insert(EngineRunTime);
}

void PIDs_10hz()
{
  insert(VehicleSpeed_PID);
  insert(FuelLevel_PID);
  insert(EngineCoolant_PID);  
}

void PIDs_20hz()
{
  
}
