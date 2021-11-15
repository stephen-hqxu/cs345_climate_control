#ifndef _COMMON_H_
#define _COMMON_H_

//Network Sim
#include <contiki.h>
#include <sys/etimer.h>

#include <net/ipv6/simple-udp.h>
#include <net/routing/routing.h>

//Log
#include <sys/log.h>
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

//Climate
#include "Climate.h"

//Port information
#define SINK_PORT 5678
#define CLIMATE_SENSOR_PORT 8765
#define ACTUATOR_PORT 12345

#endif//_COMMON_H_