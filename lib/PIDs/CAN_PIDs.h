#define PIDsupported1                                    0x00    
#define MonitorStatus                                    0x01    
#define FreezeDTC                                        0x02
#define FuelSystem                                       0x03
#define EngineLoad                                       0x04
#define EngineCollantTemp                                0x05        
#define ShortTermFuel_Bank1                              0x06        
#define LongTermFuel_Bank1                               0x07        
#define ShortTermFuel_Bank2                              0x08        
#define LongTermFuel_Bank2                               0x09        
#define FuelPressure                                     0x0A    
#define AbsolutePressure                                 0x0B    
#define EngineRPM                                        0x0C
#define VehicleSpeed                                     0x0D    
#define TimingAdvance                                    0x0E    
#define IntakeAirTemperature                             0x0F            
#define MAFairFlowRate                                   0x10    
#define ThrottlePosition                                 0x11        
#define CommandedSecondaryAirStatus                      0x12
#define OxygenSensorsPresent                             0x13
#define OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1  0x14                        
#define OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2  0x15                                               
#define OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3  0x16                                           
#define OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4  0x17                                           
#define OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1  0x18                                           
#define OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2  0x19                                           
#define OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3  0x1A                                           
#define OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4  0x1B                                           
#define OBDstandard                                      0x1C       
#define OxygenSensorsPresent2                            0x1D                   
#define AuxiliaryInputStatus                             0x1E                   
#define RunTime                                          0x1F   
#define PIDsupported2                                    0x20           
#define DistanceTraveled                                 0x21               
#define FuelRailPressure_vac                             0x22                   
#define FuelRailPressure_dis                             0x23                   
#define O2S1_WR_lambda1                                  0x24           
#define O2S2_WR_lambda1                                  0x25           
#define O2S3_WR_lambda1                                  0x26           
#define O2S4_WR_lambda1                                  0x27
#define O2S5_WR_lambda1                                  0x28                                                      
#define O2S6_WR_lambda1                                  0x29               
#define O2S7_WR_lambda1                                  0x2A               
#define O2S8_WR_lambda1                                  0x2B               
#define EGR                                              0x2C   
#define EGR_ERROR                                        0x2D           
#define EvaporativePurge                                 0x2E                   
#define FuelLevelInput                                   0x2F               
#define warm_ups                                         0x30           
#define DistanceTraveledSinceCodeCleared                 0x31                                   
#define VaporPressure                                    0x32               
#define BarometricPressure                               0x33                   
#define O2S1_WR_lambda2                                  0x34               
#define O2S2_WR_lambda2                                  0x35               
#define O2S3_WR_lambda2                                  0x36               
#define O2S4_WR_lambda2                                  0x37               
#define O2S5_WR_lambda2                                  0x38               
#define O2S6_WR_lambda2                                  0x39               
#define O2S7_WR_lambda2                                  0x3A               
#define O2S8_WR_lambda2                                  0x3B               
#define CatalystTemperature_Bank1Sensor1                 0x3C                       
#define CatalystTemperature_Bank2Sensor1                 0x3D                       
#define CatalystTemperature_Bank1Sensor2                 0x3E                       
#define CatalystTemperature_Bank2Sensor2                 0x3F                       
#define PIDsupported3                                    0x40   
#define MonitorStatusDriveCycle                          0x41           
#define ControlModuleVoltage                             0x42           
#define AbsoluteLoadValue                                0x43       
#define CommandEquivalenceRatio                          0x44           
#define RelativeThrottlePosition                         0x45               
#define AmbientAirTemperature                            0x46           
#define AbsoluteThrottlePositionB                        0x47               
#define AbsoluteThrottlePositionC                        0x48               
#define AcceleratorPedalD                                0x49       
#define AcceleratorPedalE                                0x4A       
#define AcceleratorPedalF                                0x4B       
#define CommandedThrottleActuator                        0x4C               
#define TimeRun_MIL                                      0x4D
#define TimeSinceTroubleCodesCleared                     0x4E                   
#define MaximumValueForEquivalenceRatio                  0x4F                   
#define MaximumValueForAirFlowRate                       0x50               
#define FuelType                                         0x51
#define EthanolFuel                                      0x52
#define AbsoluteVapourPressure                           0x53           
#define EvapSystemVaporPressure                          0x54           
#define ShortTermSecondaryOxygenSensor_bank1bank3        0x55                               
#define LongTermSecondaryOxygenSensor_bank1bank3         0x56                               
#define ShortTermSecondaryOxygenSensor_bank2bank4        0x57                               
#define LongTermSecondaryOxygenSensor_bank2bank4         0x58                               
#define AbsoluteFuelRailPressure                         0x59   
#define RelativeAcceleratorPedalPosition                 0x5A   
#define HybridBatteryLife                                0x5B       
#define EngineOilTemperature                             0x5C   
#define FuelInjectionTiming                              0x5D   
#define EngineFuelRate                                   0x5E       
#define EmissionRequirements                             0x5F
#define PIDsupported4                                    0X60   
#define DriverDemandEngine                               0X61       
#define ActualEngine_PercentTorque                       0X62               
#define EngineReferenceTorque                            0X63           
#define EnginePercentTorque                              0X64       
#define AuxiliaryInput                                   0X65   
#define MassAirFlowSensor                                0X66       
#define EngineCoolantTemperature                         0X67               
#define IntakeAirTemperatureSensor                       0X68               
#define CommandedEGR_ERROR                               0X69       
#define CommandedDiesel                                  0X6A   
#define ExhaustGas                                       0X6B
#define CommandedThrottleActuator2                       0X6C               
#define FuelPressureControlSystem                        0X6D               
#define InjectionPressureControl                         0X6E               
#define TurboChargerCompressor                           0X6F                           
#define BoostPressureControl                             0x70 
#define VGT                                              0x71
#define WastegateControl                                 0x72    
#define ExhaustPressure                                  0x73
#define TurbochargerRPM                                  0x74
#define TurbochargerTemperature1                         0x75
#define TurbochargerTemperature2                         0x76                
#define ChargeAIR_CACT                                   0x77            
#define EGT_Bank1                                        0x78        
#define EGT_Bank2                                        0x79        
#define DPF1                                             0x7A    
#define DPF2                                             0x7B    
#define DPF_Temperature                                  0x7C            
#define NOxNTE                                           0x7D    
#define PMxNTE                                           0x7E    
#define EngineRunTime                                    0x7F                
#define PIDsupport4                                      0x80   
#define EngineRunTime_AECD1                              0x81       
#define EngineRunTime_AECD2                              0x82       
#define NOxSensor                                        0x83
#define ManifoldSurfaceTemperature                       0x84
#define NOxReagentSystem                                 0x85
#define PM_Sensor                                        0x86
#define IntakeManifoldAbsolutePressure                   0x87
#define PIDsupport5                                      0xA0   
#define PIDsupport6                                      0xC0
