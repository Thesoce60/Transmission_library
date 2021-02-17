#ifndef mapping_H
#define mapping_H
//-------Response_State
#define State_TIMEOUT 0x84
#define State_OK 0x83
#define State_ERR 0x82
#define State_UNK 0x81

//--------------------------------
//Maquette d'émission
//[Command,[12-8] Pin,[7-0] Value]
#define Command_Pin_Mode 0x01


//--------------------------------
//Maquette d'émission
//[Command,[12-8] Pin,[7-0] Value]
#define Command_Pin_Write 0x02


//--------------------------------
//Maquette d'émission
//[Command,[12-8] Pin,[7-0] Value]
#define Command_Pin_Read 0x03


//--------------------------------
//Maquette d'émission
//[Command,[12-8] Pin,[7-0] Value]
#define Command_PWM_Change 0x04


//--------------------------------
//Maquette d'émission
//[Command,[12-8] Pin,[7-0] Value]
#define Command_Analog_Read 0x05

//--------------------------------
//Maquette d'émission
//[Command[31,23],[22-0] value]
#define Command_value 0x06

#define Command_Trigger_return 0x07


#endif