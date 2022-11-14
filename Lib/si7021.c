/*
 * si7021.c
 *
 *  Created on: October 18, 2022
 *      Author: Petar Acimovic
 */

#include "si7021.h"

//**********************************************************************
// functions

void Si7021_Reset(Si7021 * mySensor)
{
	// transmit reset command
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &RESET_SENSOR, 1, HAL_MAX_DELAY);
}

bool Si7021_IsIDLE_Temperature(Si7021 * mySensor)
{
	return !(mySensor->reading_temp);
}

bool Si7021_IsIDLE_Humidity(Si7021 * mySensor)
{
	return !(mySensor->reading_hum);
}

float Si7021_GetData(float * temp, float * hum, Si7021 * mySensor) // get relative humidity and temperature
{
	// buffers for storing data and unions
	Data data_hum;
	uint8_t buf_hum[2];
	Data data_temp;
	uint8_t buf_temp[2];

	// HAL transmit and receive for humidity
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &MEASURE_HUMIDITY, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(mySensor->i2c_handle, mySensor->address, &buf_hum, 2, HAL_MAX_DELAY);

	// putting data into the union
	data_hum.DataMembers.LowBits = buf_hum[1];
	data_hum.DataMembers.HighBits = buf_hum[0];

	// HAL transmit and receive the previous temperature so you save time
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &READ_PREVIOUS , 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(mySensor->i2c_handle, mySensor->address, &buf_temp, 2, HAL_MAX_DELAY);

	// putting data into the union
	data_temp.DataMembers.LowBits = buf_temp[1];
	data_temp.DataMembers.HighBits = buf_temp[0];

	// getting already merged data from union
	uint16_t packet_hum = data_hum.u16data;
	uint16_t packet_temp = data_temp.u16data;

	// getting the real readable numbers into the variable that were passed by reference
	*hum = (125.0 * (float)packet_hum) / 65536.0 - 6.0;
	*temp = (175.72 * (float)packet_temp) / 65536.0 - 46.85;

}

// hold master mode:

float Si7021_GetTemp(Si7021 * mySensor)
{
	// buffer for storing data
	uint8_t buf_temp[2];

	// HAL transmit and receive
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &MEASURE_TEMPERATURE , 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(mySensor->i2c_handle, mySensor->address, &buf_temp, 2, HAL_MAX_DELAY);

	// merging data into a packet
	uint16_t packet_temp = (buf_temp[0] << 8) & 0xff00; // MSB is in buf[0]
	packet_temp |= buf_temp[1] & 0x00ff; // LSB is in buf[1]

	 // getting the real readable numbers
	float temp = (175.72 * (float)packet_temp) / 65536.0 - 46.85;

	return temp;
}
float Si7021_GetRelHum(Si7021 * mySensor)
{
	// buffer for storing data
	uint8_t buf_hum[2];

	// HAL transmit and receive
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &MEASURE_HUMIDITY, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(mySensor->i2c_handle, mySensor->address, &buf_hum, 2, HAL_MAX_DELAY);

	// merging data into a packet
	uint16_t packet_hum = (buf_hum[0] << 8) & 0xff00; // MSB is in buf[0]
	packet_hum |= buf_hum[1] & 0x00ff; // LSB is in buf[1]

	// getting the real readable numbers
	float hum = (125.0 * (float)packet_hum) / 65536.0 - 6.0;

	return hum;
}

// no hold master mode:

void Si7021_Start_NoHold_Temp(Si7021 * mySensor)
{
	// HAL transmit
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &MEASURE_TEMPERATURE_NO_HOLD , 1, HAL_MAX_DELAY);
	// reading process started
	mySensor->reading_temp = true;
}

float Si7021_Get_NoHold_Temp(Si7021 * mySensor)
{
	// buffer for storing data
	uint8_t buf_temp[2];

	// receive data when it is ready (must be checked manually)
	HAL_I2C_Master_Receive(mySensor->i2c_handle, mySensor->address, &buf_temp, 2, HAL_MAX_DELAY);

	// merging data into a packet
	uint16_t packet_temp = (buf_temp[0] << 8) & 0xff00; // MSB is in buf[0]
	packet_temp |= buf_temp[1] & 0x00ff; // LSB is in buf[1]

	// getting the real readable numbers
	float temp = (175.72 * (float)packet_temp) / 65536.0 - 46.85;

	// not reading temperature anymore
	mySensor->reading_temp = false;

	return temp;
}

void Si7021_Start_NoHold_Hum(Si7021 * mySensor)
{
	// HAL transmit
	HAL_I2C_Master_Transmit(mySensor->i2c_handle, mySensor->address, &MEASURE_HUMIDITY_NO_HOLD, 1, HAL_MAX_DELAY);
	// reading process started
	mySensor->reading_hum = true;
}

float Si7021_Get_NoHold_Hum(Si7021 * mySensor)
{
	// buffer for storing data
	uint8_t buf_hum[2];

	HAL_I2C_Master_Receive(mySensor->i2c_handle, mySensor->address, &buf_hum, 2, HAL_MAX_DELAY);

	// merging data into a packet
	uint16_t packet_hum = (buf_hum[0] << 8) & 0xff00; // MSB is in buf[0]
	packet_hum |= buf_hum[1] & 0x00ff; // LSB is in buf[1]

	// getting the real readable numbers
	float hum = (125.0 * (float)packet_hum) / 65536.0 - 6.0;

	// not reading humidity anymore
	mySensor->reading_hum = false;

	return hum;
}
