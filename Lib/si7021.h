/*
 * si7021.h
 *
 *  Created on: October 18, 2022
 *      Author: Petar Acimovic
 */

#ifndef SI7021_H_
#define SI7021_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"
// for this stm32nucleo generation 4,
// the whole HAL library needs to be included

//**********************************************************************
// command codes (constants)

const static uint8_t MEASURE_HUMIDITY = 0xE5; // hold master mode
const static uint8_t MEASURE_TEMPERATURE = 0xE3; // hold master mode
const static uint8_t READ_PREVIOUS = 0xE0; // read previous temperature
const static uint8_t MEASURE_HUMIDITY_NO_HOLD = 0xF5; // no hold master mode
const static uint8_t MEASURE_TEMPERATURE_NO_HOLD = 0xF3; // no hold master mode
const static uint8_t RESET_SENSOR = 0xFE; // reset

//**********************************************************************
// structure for the sensor

typedef struct
{

	uint8_t address; //0x40 (8 bit) -> requires shifting to get 7 bit

	I2C_HandleTypeDef * i2c_handle;

	volatile bool reading_temp;

	volatile bool reading_hum;

} Si7021;

//**********************************************************************
// union for data

typedef union
{
	uint16_t u16data;

	struct u8data
	{
		uint8_t LowBits;
		uint8_t HighBits;

	} DataMembers;

} Data;

//**********************************************************************
// function prototypes

bool Si7021_IsIDLE_Temperature(Si7021 *);
bool Si7021_IsIDLE_Humidity(Si7021 *);

void Si7021_Reset(Si7021 *);

float Si7021_GetData(float *, float *, Si7021 *); // get relative humidity and temperature (unions added)

// hold master mode
float Si7021_GetTemp(Si7021 *); // get only temperature
float Si7021_GetRelHum(Si7021 *); // get relative humidity

// no hold master mode, use when time efficiency is your priority
// two sets of function - one for start reading, one for getting data when it's ready
void Si7021_Start_NoHold_Temp(Si7021 *);
void Si7021_Start_NoHold_Hum(Si7021 *);
float Si7021_Get_NoHold_Temp(Si7021 *);
float Si7021_Get_NoHold_Hum(Si7021 *);

#endif /* SI7021_H_ */
