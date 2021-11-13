#ifndef _CLIMATE_H_
#define _CLIMATE_H_

#include <stdbool.h>

/**
 * @brief Initialise climate control environment
 **/
void initClimate();

/**
 * @brief Get the temperature of the environment.
 * @return The current temperature of the environment.
 * 
 **/
float getTemperature();

/**
 * @brief Set the motor on both actuators to be either on or off.
 * @param power True to turn on, false to turn off.
 * 
 **/
void setActuatorMotor(bool);

#endif//_CLIMATE_H_
