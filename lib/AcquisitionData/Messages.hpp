#ifndef MESSAGES_H
#define MESSAGES_H

#include "AcquisitionData.h"

#define verify_char(c) (c == 10 ? 'A' : c == 11 ? 'B' \
                                      : c == 12 ? 'C' \
                                      : c == 13 ? 'D' \
                                      : c == 14 ? 'E' \
                                      : c == 15 ? 'F' : 'Z') // If c < 10, send the character 'Z' as an error flag
/* Values to the CAN_Messages class
 * 0 - Não Printa nada na Serial
 * 1 - Print apenas mensagens CAN
 * 2 - Print apenas DTC
 * 4 - Print DTC + mensagens CAN
 */
#define debug_message 0

class CAN_Messages
{
    private:
        String make_DTC_code(uint8_t first_msg, uint8_t second_msg)
        {
            String code_dtc = "";

            // First character
            char _first_char = (first_msg == 0 ? 'P' : first_msg == 1 ? 'C'         \
                                                     : first_msg == 2 ? 'B'         \
                                                     : first_msg == 3 ? 'U' : 'Z'); \
            if (_first_char == 'Z')
                return {"null"};

            code_dtc += String(_first_char); // write the first char

            uint8_t _second_char = (second_msg & 0x03);
            _second_char = (_second_char != 0 && _second_char != 3 ? _second_char ^= 3 : _second_char);
            code_dtc += String(_second_char); // write the second char
            
            char *characters = (char*)malloc(sizeof(char) * 3);
            uint8_t _third_fourth_fifth_char[] =
            {
                (uint8_t)((second_msg & 0xF0) >> 4), (uint8_t)((second_msg & 0xF0) >> 4), (uint8_t)(second_msg & 0x0F)
            };

            for (int8_t i = 0; i < sizeof(_third_fourth_fifth_char); i++)
            { // write the third, fourth and fifth char (in this order each interation)
                *(characters + i) = (char)verify_char(_third_fourth_fifth_char[i]);

                if (*(characters + i) != 'Z')
                 code_dtc += String(*(characters + i));
                else
                 code_dtc += String(_third_fourth_fifth_char[i]);
            }

            free(characters);
            return code_dtc;
        };

        String Check_type_of_fuel(uint8_t type)
        {
           switch(type)
            {
              case 0x01:
              {
                return {"Gasoline"};
                break;
              }

              case 0x02:
              {
                return {"Methanol"};
                break;
              }

              case 0x03:
              {
                return {"Ethanol"};
                break;
              }

              case 0x04:
              {
                return {"Diesel"};
                break;
              }

              case 0x05:
              {
                return {"LPG"};
                break;
              }

              case 0x06:
              {
                return {"CNG"};
                break;
              }

              case 0x07:
              {
                return {"Propane"};
                break;
              }

              case 0x08:
              {
                return {"Electric"};
                break;
              }

              case 0x09:
              {
                return {"Bifuel run Gasoline"};
                break;
              }

              case 0x0A:
              {
                return {"Bifuel run Methanol"};
                break;
              }

              case 0x0B:
              {
                return {"Bifuel run Ethanol"};
                break;
              }

              case 0x0C:
              {
                return {"Bifuel run LPG"};
                break;
              }

              case 0x0D:
              {
                return {"Bifuel run CNG"};
                break;
              }

              case 0x0E:
              {
                return {"Bifuel run Prop"};
                break;
              }

              case 0x0F:
              {
                return {"Bifuel run Electricity"};
                break;
              }

              case 0x10:
              {
                return {"Bifuel mixed gas/electric"};
                break;
              }

              case 0x11:
              {
                return {"Hybrid gasoline"};
                break;
              }

              case 0x12:
              {
                return {"Hybrid ethanol"};
                break;
              }

              case 0x13:
              {
                return {"Hybrid Diesel"};
                break;
              }

              case 0x14:
              {
                return {"Hybrid electric"};
                break;
              }

              case 0x15:
              {
                return {"Hybrid mixed fuel"};
                break;
              }

              case 0x16:
              {
                return {"Hybrid regenerative"};
                break;
              }
            }
           return "null";
        };

    public:
        CAN_Messages() {};

        ~CAN_Messages() {};

        void Handling_Message(uint8_t *PID, BLE_packet_t *packet)
        {
            switch (*(PID + 2))
            {
                case 0x41:
                { // case for PID support
                    if (*(PID + 0) == 0x10)
                    {
                        if (*(PID + 3) == PIDs1)
                            Storage_PIDenable_bit(PID, PID_to_index_1);
                        else if (*(PID + 3) == PIDs2)
                            Storage_PIDenable_bit(PID, PID_to_index_2);
                        else if (*(PID + 3) == PIDs3)
                            Storage_PIDenable_bit(PID, PID_to_index_3);
                        else if (*(PID + 3) == PIDs4)
                            Storage_PIDenable_bit(PID, PID_to_index_4);
                        else if (*(PID + 3) == PIDs5)
                            Storage_PIDenable_bit(PID, PID_to_index_5);
                    }
            
                    break;
                }

                case EngineLoad:
                {
                    float A = *(PID + 3);
                    packet->Calculated_Engine_Load = (100 * A) / 255;
                    
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Calculated engine load value:  %f\r\n", packet->Calculated_Engine_Load);
                    #endif
            
                    break;
                }
            
                case EngineCollantTemp:
                {
                    float A = *(PID + 3);
                    packet->Engine_Coolant_Temperature = A - 40;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Engine Coolant Temperature:  %f\r\n", packet->Engine_Coolant_Temperature);
                    #endif
            
                    break;
                }
            
                case ShortTermFuel_Bank1:
                {
                    float A = *(PID + 3);
                    packet->Short_Long_Term.ShortTermFuel.Bank1 = (A - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("ShortTermFuel_Bank1:  %f\r\n", packet->Short_Long_Term.ShortTermFuel.Bank1);
                    #endif

                    break;
                }

                case LongTermFuel_Bank1:
                {
                    float A = *(PID + 3);
                    packet->Short_Long_Term.LongTermFuel.Bank1 = (A - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("LongTermFuel_Bank1:  %f\r\n", packet->Short_Long_Term.LongTermFuel.Bank1);
                    #endif

                    break;
                }

                case ShortTermFuel_Bank2:
                {
                    float A = *(PID + 3);
                    packet->Short_Long_Term.ShortTermFuel.Bank2 = (A - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("ShortTermFuel_Bank2:  %f\r\n", packet->Short_Long_Term.ShortTermFuel.Bank2);
                    #endif

                    break;
                }

                case LongTermFuel_Bank2:
                {
                    float A = *(PID + 3);
                    packet->Short_Long_Term.LongTermFuel.Bank2 = (A - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("LongTermFuel_Bank2:  %f\r\n", packet->Short_Long_Term.LongTermFuel.Bank2);
                    #endif

                    break;
                }

                case FuelPressure:
                {
                    float A = *(PID + 3);
                    packet->Fuel_Pressure = 3 * A;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Fuel Pressure:  %f\r\n", packet->Fuel_Pressure);
                    #endif
            
                    break;
                }
            
                case IntakeManifoldAbsolutePressure:
                {
                    float A = *(PID + 3);
                    packet->Intake_Manifold__MAP = A;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Intake manifold absolute pressure(MAP):  %f\r\n", packet->Intake_Manifold__MAP);
                    #endif
            
                    break;
                }

                case EngineRPM:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Engine_RPM = (256 * A + B) / 4;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Engine RPM:  %f\r\n", packet->Engine_RPM);
                    #endif
            
                    break;
                }

                case VehicleSpeed:
                {
                    float A = *(PID + 3);
                    packet->Speed = A;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Vehicle speed:  %f\r\n", packet->Speed);
                    #endif
            
                    break;
                }

                case TimingAdvance:
                {
                    float A = *(PID + 3);
                    packet->Timing_Advance = (A / 2) - 64;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("TimingAdvance:  %f\r\n", packet->Timing_Advance);
                    #endif

                    break;
                }

                case IntakeAirTemperature:
                {
                    float A = *(PID + 3);
                    packet->Intake_Air_Temperature = A - 40;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("IntakeAirTemperature:  %f\r\n", packet->Intake_Air_Temperature);
                    #endif
                    
                    break;
                }

                case MAFairFlowRate:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->MAF_Air_FlowRate = ((A * 256) + B) / 100;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("MAFairFlowRate:  %f\r\n", packet->MAF_Air_FlowRate);
                    #endif

                    break;
                }      

                case ThrottlePosition:
                {
                    float A = *(PID + 3);
                    packet->Throttle_Position = (100 * A) / 255;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Throttle position:  %f\r\n", packet->Throttle_Position);
                    #endif
            
                    break;
                }
            
                case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor1 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk1_ss1 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor1);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor1 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk1_ss1);
                    }
                    #endif
                    
                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor2 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk1_ss2 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor2);
                    
                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor2 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk1_ss2);
                    }
                    #endif

                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor3 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk1_ss3 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor3);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor3 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk1_ss3);
                    }
                    #endif

                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor4 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk1_ss4 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank1_Sensor4);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank1Sensor4 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk1_ss4);
                    }
                    #endif

                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor1 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk2_ss1 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor1);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor1 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk2_ss1);
                    }
                    #endif

                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor2 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk2_ss2 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor2);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor2 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk2_ss2);
                    }
                    #endif

                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor3 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk2_ss3 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor3);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor3 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk2_ss3);
                    }
                    #endif

                    break;
                }

                case OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4:
                {
                    float A = *(PID + 3), B = *(PID + 4); 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor4 = A / 200;

                    if (B != 0xFF)
                    packet->Short_Long_Term.OxygenSensorVolt.bk2_ss4 = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4:  %f\r\n", 
                    packet->Short_Long_Term.OxygenSensorVolt._Bank2_Sensor4);

                    if (B != 0xFF)
                    {
                            Serial.printf("OxygenSensorVolt_ShortTermFuelTrim_Bank2Sensor4 ( 2 ):  %f\r\n", 
                        packet->Short_Long_Term.OxygenSensorVolt.bk2_ss4);
                    }
                    #endif

                    break;
                }

                case RunTimeSinceEngineStart:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Run_Time = 256 * A + B;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Run Time since engine start:  %f\r\n", packet->Run_Time);
                    #endif
            
                    break;
                }
            
                case DistanceTraveledMIL:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Distance_traveled_with_MIL_on = 256 * A + B;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Distance traveled with malfunction indicator lamp (MIL) on :  %f\r\n", packet->Distance_traveled_with_MIL_on);
                    #endif
            
                    break;
                }
            
                case FuelRailPressure_vac:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Fuel_Rail_Pressure_vac = ((A * 256) + B) * 0.079;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Fuel Rail Pressure_vac:  %f\r\n", packet->Fuel_Rail_Pressure_vac);
                    #endif

                    break;
                }

                case FuelRailPressure_dis:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Fuel_Rail_Pressure_dis = ((A * 256) + B) * 10;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Fuel Rail Pressure_dis:  %f\r\n", packet->Fuel_Rail_Pressure_dis);
                    #endif

                    break;
                }

                case O2S1_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S1[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S1[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S1_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S2[0], packet->Short_Long_Term.O2Sx_WR.O2S2[1]);
                    #endif

                    break;
                }

                case O2S2_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S2[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S2[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S2_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S2[0], packet->Short_Long_Term.O2Sx_WR.O2S2[1]);
                    #endif

                    break;
                }

                case O2S3_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S3[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S3[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S3_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S3[0], packet->Short_Long_Term.O2Sx_WR.O2S3[1]);
                    #endif

                    break;
                }

                case O2S4_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S4[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S4[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S4_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S4[0], packet->Short_Long_Term.O2Sx_WR.O2S4[1]);
                    #endif

                    break;
                }

                case O2S5_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S5[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S5[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S5_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S5[0], packet->Short_Long_Term.O2Sx_WR.O2S5[1]);
                    #endif

                    break;
                }

                case O2S6_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S6[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S6[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S6_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S6[0], packet->Short_Long_Term.O2Sx_WR.O2S6[1]);
                    #endif

                    break;
                }

                case O2S7_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S7[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S7[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S7_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S7[0], packet->Short_Long_Term.O2Sx_WR.O2S7[1]);
                    #endif

                    break;
                }

                case O2S8_WR_lambda1:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR.O2S8[0] = ((A * 256) + B) * (2 / 65535);
                    packet->Short_Long_Term.O2Sx_WR.O2S8[1] = ((C * 256) + D) * (8 / 65535);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S8_WR_lambda1:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR.O2S8[0], packet->Short_Long_Term.O2Sx_WR.O2S8[1]);
                    #endif

                    break;
                }

                case EGR:
                {
                    float A = *(PID + 3);
                    packet->_EGR = 100 * (A / 255);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("EGR:  %f\r\n", packet->_EGR);
                    #endif

                    break;
                }

                case EGR_ERROR:
                {
                    float A = *(PID + 3);
                    packet->_EGR_ERROR = (A - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("EGR_ERROR:  %f\r\n", packet->_EGR_ERROR);
                    #endif

                    break;
                }

                case EvaporativePurge:
                {
                    float A = *(PID + 3);
                    packet->Evaporative_Purge = 100 * (A / 255);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Evaporative Purge:  %f\r\n", packet->Evaporative_Purge);
                    #endif

                    break;
                }

                case FuelLevelInput:
                {
                    float A = *(PID + 3);
                    packet->Fuel_Level_input = (100 * A) / 255;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Fuel Level Input:  %f\r\n", packet->Fuel_Level_input);
                    #endif
            
                    break;
                }
            
                case warm_ups:
                {
                    float A = *(PID + 3);
                    packet->Warm_Ups = A;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Warm Ups:  %f\r\n", packet->Fuel_Level_input);
                    #endif

                    break;
                }

                case DistanceTraveledSinceCodeCleared:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Distance_traveled = 256 * A + B;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Distance traveled since codes cleared:  %f\r\n", packet->Distance_traveled);
                    #endif
            
                    break;
                }
            
                case VaporPressure:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Vapor_Pressure = ((A * 256) + B) / 4;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Vapor Pressure:  %f\r\n", packet->Vapor_Pressure);
                    #endif
                    
                    break;
                }

                case BarometricPressure:
                {
                    float A = *(PID + 3);
                    packet->Barometric_Pressure = A;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Barometric Pressure:  %f\r\n", packet->Barometric_Pressure);
                    #endif

                    break;
                }

                case O2S1_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S1[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S1[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S1_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S1[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S1[1]);
                    #endif

                    break;
                }

                case O2S2_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S2[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S2[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S2_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S2[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S2[1]);
                    #endif

                    break;
                }

                case O2S3_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S3[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S3[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S3_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S3[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S3[1]);
                    #endif

                    break;
                }

                case O2S4_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S4[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S4[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S4_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S4[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S4[1]);
                    #endif

                    break;
                }

                case O2S5_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S5[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S5[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S5_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S5[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S5[1]);
                    #endif

                    break;
                }

                case O2S6_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S6[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S6[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S6_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S6[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S6[1]);
                    #endif

                    break;
                }

                case O2S7_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S7[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S7[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S7_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S7[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S7[1]);
                    #endif

                    break;
                }

                case O2S8_WR_lambda2:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S8[0] = ((A * 256) + B) / 32.768;
                    packet->Short_Long_Term.O2Sx_WR_lambda2.O2S8[1] = (((C * 256) + D) / 256) - 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("O2S8_WR_lambda2:   %f | %f\r\n", 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S8[0], 
                        packet->Short_Long_Term.O2Sx_WR_lambda2.O2S8[1]);
                    #endif

                    break;
                }

                case CatalystTemperature_Bank1Sensor1:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Catalyst_Temperature.Bank1_Sensor1 = (((A * 256) + B) / 10) - 40;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Catalyst Temperature Bank1_Sensor1:  %f\r\n", 
                        packet->Short_Long_Term.Catalyst_Temperature.Bank1_Sensor1);
                    #endif

                    break;
                }

                case CatalystTemperature_Bank2Sensor1:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Catalyst_Temperature.Bank2_Sensor1 = (((A * 256) + B) / 10) - 40;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Catalyst Temperature Bank2_Sensor1:  %f\r\n", 
                        packet->Short_Long_Term.Catalyst_Temperature.Bank2_Sensor1);
                    #endif

                    break;
                }
                
                case CatalystTemperature_Bank1Sensor2:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Catalyst_Temperature.Bank1_Sensor2 = (((A * 256) + B) / 10) - 40;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Catalyst Temperature Bank1_Sensor2:  %f\r\n", 
                        packet->Short_Long_Term.Catalyst_Temperature.Bank1_Sensor2);
                    #endif

                    break;
                }
                
                case CatalystTemperature_Bank2Sensor2:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Catalyst_Temperature.Bank2_Sensor2 = (((A * 256) + B) / 10) - 40;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Catalyst Temperature Bank2_Sensor2:  %f\r\n", 
                        packet->Short_Long_Term.Catalyst_Temperature.Bank2_Sensor2);
                    #endif

                    break;
                }

                case ControlModuleVoltage:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Control_Module_Voltage = ((A * 256) + B) / 1000;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Control Module Voltage:  %f\r\n", packet->Control_Module_Voltage);
                    #endif                    

                    break;
                }

                case AbsoluteLoadValue:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Absolute_Load_Value = ((A * 256) + B) * (100 / 255);
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Load Value:  %f\r\n", packet->Absolute_Load_Value);
                    #endif                    

                    break;
                }

                case CommandEquivalenceRatio:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Command_Equivalence_Ratio = ((A * 256) + B) / 32.768;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Command Equivalence Ratio:  %f\r\n", packet->Command_Equivalence_Ratio);
                    #endif                    

                    break;
                }

                case RelativeThrottlePosition:
                {
                    float A = *(PID + 3);
                    packet->Relative_Throttle_Position = (A * 100) / 255;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Relative Throttle Position:  %f\r\n", packet->Relative_Throttle_Position);
                    #endif                    

                    break;
                }

                case AmbientAirTemperature:
                {
                    float A = *(PID + 3);
                    packet->Ambient_Air_Temperature = A - 40;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Ambient air temperature:  %f\r\n", packet->Ambient_Air_Temperature);
                    #endif
            
                    break;
                }

                case AbsoluteThrottlePositionB:
                {
                    float A = *(PID + 3);
                    packet->Absolute_Throttle_PositionB = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Throttle PositionB:  %f\r\n", packet->Absolute_Throttle_PositionB);
                    #endif

                    break;
                }

                case AbsoluteThrottlePositionC:
                {
                    float A = *(PID + 3);
                    packet->Absolute_Throttle_PositionC = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Throttle PositionC:  %f\r\n", packet->Absolute_Throttle_PositionC);
                    #endif

                    break;
                }

                case AcceleratorPedalPositionD:
                {
                    float A = *(PID + 3);
                    packet->Absolute_Throttle_PositionD = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Throttle PositionD:  %f\r\n", packet->Absolute_Throttle_PositionD);
                    #endif

                    break;
                }

                case AcceleratorPedalPositionE:
                {
                    float A = *(PID + 3);
                    packet->Absolute_Throttle_PositionE = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Throttle PositionE:  %f\r\n", packet->Absolute_Throttle_PositionE);
                    #endif

                    break;
                }

                case AcceleratorPedalPositionF:
                {
                    float A = *(PID + 3);
                    packet->Absolute_Throttle_PositionF = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Throttle PositionF:  %f\r\n", packet->Absolute_Throttle_PositionF);
                    #endif

                    break;
                }

                case CommandedThrottleActuator:
                {
                    float A = *(PID + 3);
                    packet->Commanded_Throttle_Actuator = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Commanded Throttle Actuator:  %f\r\n", packet->Commanded_Throttle_Actuator);
                    #endif

                    break;
                }

                case TimeRun_MIL:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Time_Run_MIL = (A * 256) + B;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Time Run MIL:  %f\r\n", packet->Time_Run_MIL);
                    #endif

                    break;
                }

                case TimeSinceTroubleCodesCleared:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Time_Since_Trouble_Codes_Cleared = (A * 256) + B;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("TimeS ince Trouble Codes Cleared:  %f\r\n", 
                        packet->Time_Since_Trouble_Codes_Cleared);
                    #endif

                    break;
                }

                case MaximumValueForEquivalenceRatio:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Maximum_Value_For_Equivalence_Ratio[0] = A;
                    packet->Maximum_Value_For_Equivalence_Ratio[1] = B;
                    packet->Maximum_Value_For_Equivalence_Ratio[2] = C;
                    packet->Maximum_Value_For_Equivalence_Ratio[3] = D * 10;
                                                    
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Maximum Value For Equivalence Ratio:   %f | %f | %f | %f\r\n",
                        packet->Maximum_Value_For_Equivalence_Ratio[0],
                        packet->Maximum_Value_For_Equivalence_Ratio[1],
                        packet->Maximum_Value_For_Equivalence_Ratio[2],
                        packet->Maximum_Value_For_Equivalence_Ratio[3]);
                    #endif

                    break;
                }

                case MaximumValueForAirFlowRate:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5);
                    packet->Maximum_Value_For_Air_Flow_Rate[0] = A * 10;
                    packet->Maximum_Value_For_Air_Flow_Rate[1] = B;
                    packet->Maximum_Value_For_Air_Flow_Rate[2] = C;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Maximum Value For Air Flow Rate:   %f | %f | %f\r\n",
                        packet->Maximum_Value_For_Air_Flow_Rate[0],
                        packet->Maximum_Value_For_Air_Flow_Rate[1],
                        packet->Maximum_Value_For_Air_Flow_Rate[2]);
                    #endif
    
                    break;
                }

                case FuelType:
                {
                    float A = *(PID + 3);
                    packet->Fuel_Type = this->Check_type_of_fuel(A);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Fuel Type:  ");
                        Serial.println(packet->Fuel_Type);
                    #endif

                    break;
                }

                case EthanolFuel:
                {
                    float A = *(PID + 3);
                    packet->Ethanol_Fuel = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Ethanol_Fuel:  %f\r\n", packet->Ethanol_Fuel);
                    #endif

                    break;
                }

                case AbsoluteVapourPressure:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Absolute_Vapour_Pressure = (A / 200) + (B / 200) + (C / 200) + (D / 200);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Vapour Pressure:  %f\r\n", packet->Absolute_Vapour_Pressure);
                    #endif

                    break;
                }

                case EvapSystemVaporPressure:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Evap_System_Vapor_Pressure = ((A * 256) + B) - 32.768;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Evap System Vapor Pressure:  %f\r\n", packet->Evap_System_Vapor_Pressure);
                    #endif

                    break;
                }

                case ShortTermSecondaryOxygenSensor_bank1bank3:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank1_bank3[0] = (A - 128) * (100 / 128); 
                    packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank1_bank3[1] = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("ShortTerm Secondary Oxygen Sensor Bank1_Bank3:  %f | %f\r\n", 
                        packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank1_bank3[0],
                        packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank1_bank3[1]);
                    #endif

                    break;
                }

                case LongTermSecondaryOxygenSensor_bank1bank3:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank1_bank3[0] = (A - 128) * (100 / 128);
                    packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank1_bank3[1] = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("LongTerm Secondary Oxygen Sensor Bank1_Bank3:  %f | %f\r\n", 
                        packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank1_bank3[0],
                        packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank1_bank3[1]);
                    #endif

                    break;
                }

                case ShortTermSecondaryOxygenSensor_bank2bank4:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank2_bank4[0] = (A - 128) * (100 / 128);
                    packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank2_bank4[1] = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("ShortTerm Secondary Oxygen Sensor Bank2_Bank4:  %f | %f\r\n", 
                        packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank2_bank4[0],
                        packet->Short_Long_Term.Secondary_OxygenSensor.ShortTerm_bank2_bank4[1]);
                    #endif

                    break;
                }

                case LongTermSecondaryOxygenSensor_bank2bank4:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank2_bank4[0] = (A - 128) * (100 / 128);
                    packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank2_bank4[1] = (B - 128) * (100 / 128);

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("LongTerm Secondary Oxygen Sensor Bank2_Bank4:  %f | %f\r\n", 
                        packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank2_bank4[0],
                        packet->Short_Long_Term.Secondary_OxygenSensor.LongTerm_bank2_bank4[1]);
                    #endif

                    break;
                }
                
                case AbsoluteFuelRailPressure:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Absolute_Fuel_Rail_Pressure = ((A * 256) + B) * 10; 

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Absolute Fuel Rail Pressure:  %f\r\n", packet->Absolute_Fuel_Rail_Pressure);
                    #endif

                    break;
                }
                
                case RelativeAcceleratorPedalPosition:
                {
                    float A = *(PID + 3);
                    packet->Relative_Accelerator_Pedal_Position = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Relative Accelerator Pedal Position:  %f\r\n", 
                        packet->Relative_Accelerator_Pedal_Position);
                    #endif

                    break;
                }

                case HybridBatteryLife:
                {
                    float A = *(PID + 3);
                    packet->Hybrid_Battery_Life = (A * 100) / 255;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Hybrid Battery Life:  %f\r\n", packet->Hybrid_Battery_Life);
                    #endif

                    break;
                }

                case EngineOilTemperature:
                {
                    float A = *(PID + 3);
                    packet->Engine_Oil_Temperature = A - 40;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Engine Oil Temperature:  %f\r\n", packet->Engine_Oil_Temperature);
                    #endif
            
                    break;
                }

                case FuelInjectionTiming:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Fuel_Injection_Timing = (((A * 256) + B) - 26.880) / 128;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Fuel Injection Timing:  %f\r\n", packet->Fuel_Injection_Timing);
                    #endif

                    break;
                }

                case EngineFuelRate:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Engine_fuel_rate = (256 * A + B) / 20;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Engine Fuel rate:  %f\r\n", packet->Engine_fuel_rate);
                    #endif
            
                    break;
                }
            
                case DriverDemandEngine:
                {
                    float A = *(PID + 3);
                    packet->Driver_Demand_Engine = A - 125;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Driver Demand Engine:  %f\r\n", packet->Driver_Demand_Engine);
                    #endif

                    break;
                }

                case ActualEngine_PercentTorque:
                {
                    float A = *(PID + 3);
                    packet->Actual_Engine_Percent_Torque = A - 125;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Actual Engine Percent Torque:  %f\r\n", packet->Actual_Engine_Percent_Torque);
                    #endif

                    break;
                }

                case EngineReferenceTorque:
                {
                    float A = *(PID + 3), B = *(PID + 4);
                    packet->Engine_Reference_Torque = (A * 256) + B;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Engine Reference Torque:  %f\r\n", packet->Engine_Reference_Torque);
                    #endif

                    break;
                }

                case EnginePercentTorque:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Engine_Percent_Torque[0] = A - 125;
                    packet->Engine_Percent_Torque[1] = B - 125;
                    packet->Engine_Percent_Torque[2] = C - 125;
                    packet->Engine_Percent_Torque[3] = D - 125;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Engine Percent Torque:  %f | %f | %f | %f\r\n", 
                        packet->Engine_Percent_Torque[0], packet->Engine_Percent_Torque[1],
                        packet->Engine_Percent_Torque[2], packet->Engine_Percent_Torque[3]);
                    #endif

                    break;
                }
                
                case MassAirFlowSensor:
                {
                    float A = *(PID + 3);
                    packet->Mass_Air_Flow_Sensor = A;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Mass Air Flow Sensor:  %f\r\n", packet->Mass_Air_Flow_Sensor);
                    #endif

                    break;
                }
                
                case EngineCoolantTemperature:
                {
                    float B = *(PID + 4), C = *(PID + 5);
                    if((*(PID + 3) & 1) == 1)
                        packet->Engine_Coolant_Temp[0] = B - 40;
                    if(((*(PID + 3) & 2) << 1) == 1)
                        packet->Engine_Coolant_Temp[1] = C - 40;

                    #if debug_message == 1 || debug_message == 4
                        if ((*(PID + 3) & 1) == 1)
                            Serial.printf("Engine Coolant Temperature B:  %f\r\n", packet->Engine_Coolant_Temp[0]);
                        if(((*(PID + 3) & 2) << 1) == 1)
                            Serial.printf("Engine Coolant Temperature C:  %f\r\n", packet->Engine_Coolant_Temp[1]);
                    #endif

                    break;
                }

                case IntakeAirTemperatureSensor:
                {
                    float A = *(PID + 3);
                    packet->Intake_Air_Temperature_Sensor = A;
                    
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Intake Air Temperature Sensor:  %f\r\n", packet->Intake_Air_Temperature_Sensor);
                    #endif

                    break;
                }

                /*
                case CommandedEGR_ERROR:
                {
                    float A = *(PID + 3);
                    packet->Commanded_EGR_ERROR = A;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Commanded EGR_ERROR:  %f\r\n", packet->Commanded_EGR_ERROR);
                    #endif

                    break;
                }

                case CommandedDiesel:
                {
                    float A = *(PID + 3);
                    packet->Commanded_Diesel = A;
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Commanded Diesel:  %f\r\n", packet->Commanded_Diesel);
                    #endif

                    break;
                }
                */

                case DPF_Temperature:
                {
                    float A = *(PID + 3);
                    packet->DPF__Temperature = A;

                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("DPF Temperature:  %f\r\n", packet->DPF__Temperature);
                    #endif

                    break;
                }

                case Odometer_PID:
                {
                    float A = *(PID + 3), B = *(PID + 4), C = *(PID + 5), D = *(PID + 6);
                    packet->Odometer = ((A * pow(2, 24)) + (B * pow(2, 16)) + (C * pow(2, 8)) + D) / 10;
            
                    #if debug_message == 1 || debug_message == 4
                        Serial.printf("Odometer:  %f\r\n", packet->Odometer);
                    #endif
            
                    break;
                }
            
                default:
                {
                    if (*(PID + 1) == 0x43)
                    {
                        packet->DTC = this->make_DTC_code(*(PID + 3), *(PID + 4));
            
                        #if debug_message == 2 || debug_message == 4
                            Serial.print("Codigo de leitura de falhas: ");
                            Serial.println(packet->DTC);
                        #endif
                    }
            
                    break;
                }
            }
        };
};

#endif
