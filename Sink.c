#include "Common.h"

//System
#include <stdarg.h>

static struct simple_udp_connection multicast_toClimate_conn;
static struct simple_udp_connection multicast_toActuator_conn;

/* Auxiliary Functions */

/**
 * @brief Calculate the mean temperature from various inputs.
 * @param count The number of temperature data to be calculated.
 * @param ... All temperature values.
 **/
static float calcTempMean(unsigned int count, ...){
    va_list args;
    va_start(args, count);

    //sum all arguments
    float sum = 0.0f;
    while(count != 0){
        sum += va_arg(args, float);
    }

    va_end(args);
    //calculate mean
    return sum / (float)count;
}

PROCESS(sinkProcess, "Sink");
AUTOSTART_PROCESSES(&sinkProcess);

static void sink_climate_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

}

static void sink_actuator_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

}

PROCESS_THREAD(sinkProcess, ev, data){
    static struct etimer sender_delay;
    uip_ipaddr_t addr;
    static char msg[32];

    PROCESS_BEGIN();

    //Initialise UDP connection from the sink to two types of end-point nodes
    if(!simple_udp_register(&multicast_toClimate_conn, SINK_PORT, NULL, CLIMATE_SENSOR_PORT, &sink_climate_rx_callback)){
        LOG_ERR("UDP connection from the sink to climate sensor could not be setup.");
        PROCESS_EXIT();
    }
    if(!simple_udp_register(&multicast_toActuator_conn, SINK_PORT, NULL, ACTUATOR_PORT, &sink_actuator_rx_callback)){
        LOG_ERR("UDP connection from the sink to actuator could not be setup.");
        PROCESS_EXIT();
    }

    //this timer is used for the sink to check if all sensors are alive
    etimer_set(&sender_delay, CLOCK_SECOND * 60);
    while(true){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));
        etimer_reset(&sender_delay);
    }

    PROCESS_END();
}