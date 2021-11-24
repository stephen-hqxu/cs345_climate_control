#ifndef _COMMON_H_
#define _COMMON_H_

//System
#include <stdbool.h>

//Network Sim
#include <contiki.h>
#include <sys/etimer.h>

#include <net/routing/routing.h>
#include <net/netstack.h>
#include <net/ipv6/simple-udp.h>

#include <sys/energest.h>

//Log
#include <sys/log.h>
#define LOG_LEVEL LOG_LEVEL_INFO

//Climate
#include "Climate.h"

//Port information
#define SINK_PORT 5678
#define RELAY_PORT 13579
#define CLIMATE_SENSOR_PORT 8765
#define ACTUATOR_PORT 24680

#define ACTUATOR_CLIMATE_BROADCAST_PORT 24678

#define ACTUATOR_STATUS_REQUEST "?"
#define ACTUATOR_ACK "!"
#define ACTUATOR_ON "On"
#define ACTUATOR_OFF "Off"

#define SINK_ACK "!"
#define SINK_STATUS_REQUEST "?"

//information about the topology
#define CLIMATE_SENSOR_COUNT 4u
#define ACTUATOR_CONTROLLER_COUNT 2u
//climate control range given by specification
#define DELTA 17u
#define GAMMA 22u

#endif//_COMMON_H_