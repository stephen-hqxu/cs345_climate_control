#ifndef _CLIMATE_H_
#define _CLIMATE_H_

#include <stdbool.h>

/**
 * @breif Init climate environment.
 **/
void initClimate();

/**
 * @brief Read the temperature of the environment using the sensor.
 * @return The temperature in Cel.
 **/
float getTemperature();

/**
 * @brief Set the motor on both actuators to be either on or off.
 * @param power True to turn on, false to turn off.
 * 
 **/
void setActuatorMotor(bool);

#endif//_CLIMATE_H_
