//Timer
#include <sys/ctimer.h>

#include <lib/random.h>

#define TEMP_DIFFERENCE_BOUND 1u

//-------------- Internal Functions ---------------------

//record climate changes based on the timer
static struct ctimer TempTimer;
static float Temperature;
static bool ActuatorMotor;

static inline float getNext(unsigned int bound){
	//A random number between 0 and bound
	return (float)random_rand() / (float)RANDOM_RAND_MAX;
}

static void timerCallback(void* ptr){
	//reset the timer
	ctimer_reset(&TempTimer);

	//When both actuators are off, then the temperature rises by approximately 1◦C every 10 minutes.
	const static float RiseRate = +1.0f / (6.0f * 10.0f);
	//When both actuators are on, the temperature drops by approximately 1◦C every 15 minutes.
	const static float DropRate = -1.0f / (6.0f * 15.0f);

	Temperature += ActuatorMotor ? DropRate : RiseRate;
}

//-------------- Header Functions ---------------------
#include "Climate.h"

void initClimate(){
	//setup timer based on AC status
	//we set the callback to every 10 seconds
	ctimer_set(&TempTimer, CLOCK_SECOND * 10, &timerCallback, NULL);

	//From the specification:
	//We assume that the temperature of the office is
	//20◦C at the start of work in the morning
	Temperature = 20.0f;
	//the actuators (i.e., both ACs or fans) are off.
	setActuatorMotor(false);
}

float getTemperature(){
	//Add some noise to the temperature retrieved by different temperature sensors
	return Temperature + getNext(TEMP_DIFFERENCE_BOUND);
}

void setActuatorMotor(bool power){
	ActuatorMotor = power;
}