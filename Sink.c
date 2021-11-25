#include "Common.h"

#define LOG_MODULE "Sink"

//System
#include <stdlib.h>
#include <string.h>

static struct simple_udp_connection toClimateSensor_conn, toClimateSensor_mcastconn;
static struct simple_udp_connection toActuator_conn, toActuator_mcastconn;

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

        if(meanTemp <= DELTA || meanTemp >= GAMMA){
            static char control[8];
            //temperature is not in the comfort range, we need to adjust the ACs
            LOG_INFO("Mean temperature is outside the comfort range, request AC control.\n");

            //all actuators are online, tell them to turn on/off
            LOG_INFO("Ready to route temperature to %d AC units.\n", ACTUATOR_CONTROLLER_COUNT);

            //determine should AC be turned on or off
            snprintf(control, sizeof(control), (meanTemp <= DELTA) ? ACTUATOR_OFF : ACTUATOR_ON);
            simple_udp_send(&toActuator_mcastconn, control, strlen(control));
        }

    }
}

static void sink_actuator_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
    //record the previous actuator state to avoid re-sending information to the climate sensor
    static bool previousStatus = false;

    LOG_INFO("Received multicast reply from actuator with address ");
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    //read the actuator status
    const char* status = (char*)data;
    bool currentStatus;
    if(strncmp(status, ACTUATOR_ON, data_length) == 0){
        currentStatus = true;

    }else if(strncmp(status, ACTUATOR_OFF, data_length) == 0){
        currentStatus = false;
    }

    if(currentStatus != previousStatus){
        //status has changed, notify all climate sensors
        simple_udp_send(&toClimateSensor_mcastconn, data, data_length);

        LOG_INFO("Current actuator status is different from the previous, route updated message to climate sensors.\n");
    }
    //otherwise there is no need to resend the same status to save energy (of climate sensors)
    LOG_INFO("Current actuator status is the same as the previous, no message sent to climate sensors.\n");
}

static void sink_climate_mcast_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

}

static void sink_actuator_mcast_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
    
}

PROCESS_THREAD(sinkProcess, ev, data){
    static uip_ipaddr_t ac_mcastAddr, climate_sensor_mcastAddr;

    PROCESS_BEGIN();

    //Sink is the source of the routing tree
    NETSTACK_ROUTING.root_start();

    //Initialise UDP connection from the sink to two types of end-point nodes
    if(!simple_udp_register(&toClimateSensor_conn, SINK_PORT, NULL, CLIMATE_SENSOR_PORT, &sink_climate_rx_callback)){
        LOG_ERR("UDP connection from the sink to climate sensor could not be setup.\n");
        PROCESS_EXIT();
    }
    if(!simple_udp_register(&toActuator_conn, SINK_PORT, NULL, ACTUATOR_PORT, &sink_actuator_rx_callback)){ LOG_INFO("Received actuator callback.\n");
        LOG_ERR("UDP connection from the sink to actuator could not be setup.\n");
        PROCESS_EXIT();
    }

    //set mcast group address to climate sensor
    SINK_CLIMATE_SENSOR_MCAST_GROUP_ADDR(climate_sensor_mcastAddr);
    if(!simple_udp_register(&toClimateSensor_mcastconn, SINK_PORT, &climate_sensor_mcastAddr, CLIMATE_SENSOR_PORT, &sink_climate_mcast_callback)){
        LOG_ERR("UDP multicast connection from the sink to climate sensor could not be setup.\n");
        PROCESS_EXIT();
    }
    //set mcast group address to AC
    ACTUATOR_SINK_MCAST_GROUP_ADDR(ac_mcastAddr);
    if(!simple_udp_register(&toActuator_mcastconn, SINK_PORT, &ac_mcastAddr, ACTUATOR_PORT, &sink_actuator_mcast_callback)){
        LOG_ERR("UDP multicast connection from the sink to actuator could not be setup.\n");
        PROCESS_EXIT();
    }

    while(true){
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}