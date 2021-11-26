#ifndef _COMMON_H_
#define _COMMON_H_

//System
#include <stdbool.h>

//Network Sim
#include <contiki.h>
#include <contiki-lib.h>
#include <contiki-net.h>

#include <sys/etimer.h>

#include <net/routing/routing.h>
#include <net/netstack.h>
#include <net/ipv6/simple-udp.h>
#include <net/ipv6/multicast/uip-mcast6.h>

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

#define ACTUATOR_SINK_MCAST_GROUP_ADDR(DEST) uip_ip6addr(&DEST, 0xFF1E, 0, 0, 0, 0, 0, 0x89, 0xABCD)
#define SINK_CLIMATE_SENSOR_MCAST_GROUP_ADDR(DEST) uip_ip6addr(&DEST, 0xFF1E, 0, 0, 0, 0, 0, 0xDC, 0xBA98)

#define ACTUATOR_ON "On"
#define ACTUATOR_OFF "Off"

//information about the topology
#define CLIMATE_SENSOR_COUNT 4u
#define ACTUATOR_CONTROLLER_COUNT 2u
//climate control range given by specification
#define DELTA 17u
#define GAMMA 22u

#endif//_COMMON_H_