#ifndef PACKETS_H
#define PACKETS_H

#include <sys/_stdint.h>

typedef struct
{
    float acc_x, acc_y, acc_z;
} imu_acc_t;

typedef struct
{
    float ang_x, ang_y, ang_z;
} imu_ang_t;

typedef struct
{
    double LAT, LNG;
} gps_data_t;

typedef struct
{
    float Bank1, Bank2;
} ShortTermFuel_t;

typedef struct
{
    float Bank1, Bank2;
} LongTermFuel_t;

typedef struct 
{
    float _Bank1_Sensor1, _Bank1_Sensor2, _Bank1_Sensor3, _Bank1_Sensor4,
    _Bank2_Sensor1, _Bank2_Sensor2, _Bank2_Sensor3, _Bank2_Sensor4;

    float bk1_ss1, bk1_ss2, bk1_ss3, bk1_ss4,
    bk2_ss1, bk2_ss2, bk2_ss3, bk2_ss4;
} OxygenSensorVolt_Short_Term_FuelTrim_t;

typedef struct
{
    float O2S1[2], O2S2[2], O2S3[2], O2S4[2], O2S5[2], O2S6[2], O2S7[2], O2S8[2];
} O2Sx_WR_lambda1_t;

typedef struct
{
    float O2S1[2], O2S2[2], O2S3[2], O2S4[2], O2S5[2], O2S6[2], O2S7[2], O2S8[2];
} O2Sx_WR_lambda2_t;

typedef struct
{
    float Bank1_Sensor1, Bank2_Sensor1, Bank1_Sensor2, Bank2_Sensor2; 
} CatalystTemperature_t;

typedef struct
{
    float   ShortTerm_bank1_bank3[2], LongTerm_bank1_bank3[2], ShortTerm_bank2_bank4[2], LongTerm_bank2_bank4[2];    
} SecondaryOxygenSensor_t;

typedef struct
{
    float Calculated_Engine_Load,

        Engine_Coolant_Temperature,

        Fuel_Pressure,

        Intake_Manifold__MAP,

        Engine_RPM,

        Speed,

        Throttle_Position,

        Run_Time,

        Distance_traveled_with_MIL_on,

        Fuel_Level_input,

        Distance_traveled,

        Ambient_Air_Temperature,

        Engine_Oil_Temperature,

        Engine_fuel_rate,

        Timing_Advance,

        Intake_Air_Temperature,

        MAF_Air_FlowRate,

        Fuel_Rail_Pressure_vac,

        Fuel_Rail_Pressure_dis,

        _EGR,

        _EGR_ERROR,

        Evaporative_Purge,

        Warm_Ups,

        Vapor_Pressure,

        Barometric_Pressure,

        Control_Module_Voltage,

        Absolute_Load_Value,

        Command_Equivalence_Ratio,

        Relative_Throttle_Position,

        Absolute_Throttle_PositionB,

        Absolute_Throttle_PositionC,

        Absolute_Throttle_PositionD,

        Absolute_Throttle_PositionE,

        Absolute_Throttle_PositionF,

        Commanded_Throttle_Actuator,

        Time_Run_MIL,

        Time_Since_Trouble_Codes_Cleared,

        Maximum_Value_For_Equivalence_Ratio[4],

        Maximum_Value_For_Air_Flow_Rate[3],

        Ethanol_Fuel,

        Absolute_Vapour_Pressure,

        Evap_System_Vapor_Pressure,

        Absolute_Fuel_Rail_Pressure,

        Relative_Accelerator_Pedal_Position,

        Hybrid_Battery_Life,

        Fuel_Injection_Timing,

        Driver_Demand_Engine,

        Actual_Engine_Percent_Torque,

        Engine_Reference_Torque,

        Engine_Percent_Torque[4],

        Mass_Air_Flow_Sensor,

        Engine_Coolant_Temp[2],

        Intake_Air_Temperature_Sensor,

        Commanded_EGR_ERROR,

        Commanded_Diesel,

        DPF__Temperature,

        Odometer,

        acctemp;
    
    struct {
        ShortTermFuel_t ShortTermFuel;
        LongTermFuel_t LongTermFuel; 
        OxygenSensorVolt_Short_Term_FuelTrim_t OxygenSensorVolt;
        O2Sx_WR_lambda1_t O2Sx_WR;
        O2Sx_WR_lambda2_t O2Sx_WR_lambda2;
        CatalystTemperature_t Catalyst_Temperature;
        SecondaryOxygenSensor_t Secondary_OxygenSensor;
    } Short_Long_Term;

    imu_acc_t imu_acc;

    imu_ang_t imu_ang;

    gps_data_t gps_data;

    String DTC, Fuel_Type;

} BLE_packet_t;

#endif
