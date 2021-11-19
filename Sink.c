#include "Common.h"

#define LOG_MODULE "Sink"

//System
#include <stdlib.h>

static struct simple_udp_connection toClimateSensor_conn;
static struct simple_udp_connection toActuator_conn;

/* Auxiliary Functions */

/**
 * @brief Calculate the mean temperature from various inputs.
 * @param count The number of temperature data to be calculated.
 * @param termps An array of all temperature values.
 **/
static float calcTempMean(unsigned int count, const float* temps){
    float sum = 0.0f;
    //sum all temps
    for(unsigned int i = 0u; i < count; i++){
        sum += temps[i];
    }
    //calculate mean
    return sum / (float)count;
}

PROCESS(sinkProcess, "Sink");
AUTOSTART_PROCESSES(&sinkProcess);

#define CLIMATE_SENSOR_COUNT 4u

static void sink_climate_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
    //record temperatures, and when we have 4 temp readings from all sensors we can calculate mean
    static unsigned int StackIdx = 0u;
    static float TempStack[CLIMATE_SENSOR_COUNT];
    
    const char* reading = (char*)data;
    LOG_INFO("Received sensor reading '%.*s' from ", data_length, reading);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    TempStack[StackIdx] = (float)atof(reading);
    StackIdx = (StackIdx + 1u) % CLIMATE_SENSOR_COUNT;
    if(StackIdx == 0u){
        //we have collected reading from all 4 sensors, can start calculating mean
        const float meanTemp = calcTempMean(CLIMATE_SENSOR_COUNT, TempStack);
        LOG_INFO("Received all temperature from %d climate sensors, mean temperature is %f\n", CLIMATE_SENSOR_COUNT, meanTemp);
    }
}

static void sink_actuator_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
    
}

PROCESS_THREAD(sinkProcess, ev, data){
    //static struct etimer sender_delay;
    //uip_ipaddr_t addr;
    //static char msg[32];

    PROCESS_BEGIN();

    //Sink is the source of the routing tree
    NETSTACK_ROUTING.root_start();

    //Initialise UDP connection from the sink to two types of end-point nodes
    if(!simple_udp_register(&toClimateSensor_conn, SINK_PORT, NULL, CLIMATE_SENSOR_PORT, &sink_climate_rx_callback)){
        LOG_ERR("UDP connection from the sink to climate sensor could not be setup.\n");
        PROCESS_EXIT();
    }
    if(!simple_udp_register(&toActuator_conn, SINK_PORT, NULL, ACTUATOR_PORT, &sink_actuator_rx_callback)){
        LOG_ERR("UDP connection from the sink to actuator could not be setup.\n");
        PROCESS_EXIT();
    }

    //this timer is used for the sink to check if all sensors are alive
    // etimer_set(&sender_delay, CLOCK_SECOND * 2);
    // while(true){
    //     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));

    //     etimer_reset(&sender_delay);
    // }

    PROCESS_END();
}