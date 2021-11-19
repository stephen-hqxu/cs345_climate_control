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

#endif//_COMMON_H_