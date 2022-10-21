#ifndef CUSTOM_PDO_NAME_H
#define CUSTOM_PDO_NAME_H

//-------------------------------------------------------------------//
//                                                                   //
//     This file has been created by the Easy Configurator tool      //
//                                                                   //
//     Easy Configurator project myCustomSlave.prj
//                                                                   //
//-------------------------------------------------------------------//


#define CUST_BYTE_NUM_OUT	19
#define CUST_BYTE_NUM_IN	7
#define TOT_BYTE_NUM_ROUND_OUT	20
#define TOT_BYTE_NUM_ROUND_IN	8


typedef union												//---- output buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_OUT];
	struct
	{
		int64_t     myOutput_4;
		uint32_t    myOutput_2;
		float       TheLastOutput;
		int16_t     myOutput_3;
		uint8_t     myOutput_1;
	}Cust;
} PROCBUFFER_OUT;


typedef union												//---- input buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_IN];
	struct
	{
		float       anOtherInput;
		int16_t     myInput_1;
		int8_t      myInput_2;
	}Cust;
} PROCBUFFER_IN;

#endif