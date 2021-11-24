#include "Common.h"

#define LOG_MODULE "Sink"

//System
#include <stdlib.h>
#include <string.h>

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

static uip_ipaddr_t ActuatorStack[ACTUATOR_CONTROLLER_COUNT];
static size_t ActuatorStackSize;

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

            if(ActuatorStackSize == ACTUATOR_CONTROLLER_COUNT){
                //all actuators are online, tell them to turn on/off
                LOG_INFO("Ready to route temperature to %d AC units.\n", ActuatorStackSize);

                //determine should AC be turned on or off
                snprintf(control, sizeof(control), (meanTemp <= DELTA) ? ACTUATOR_OFF : ACTUATOR_ON);
                for(unsigned int i = 0u; i < ActuatorStackSize; i++){
                    simple_udp_sendto(&toActuator_conn, control, strlen(control), ActuatorStack + i);
                }

            }else{
                LOG_WARN("Unable to connect to all AC units in the environment yet, because only %d is found.\n", ActuatorStackSize);
            }
        }

    }
}

static void sink_actuator_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
    
    const char* msg = (char*)data;
    //get the status code
    if(strncmp(msg, ACTUATOR_STATUS_REQUEST, data_length) == 0){
        //actuator sends regular updates to notify the sink it sif still alive
        LOG_INFO("Received actuator status from ");
        LOG_INFO_6ADDR(sender_addr);
        LOG_INFO_("\n");
        
        bool registered = ActuatorStackSize > 0ull;
        for(unsigned int i = 0u; i < ActuatorStackSize; i++){
            //check if this is an actuator that the sink has never known before
            if(!uip_ipaddr_cmp(ActuatorStack + i, sender_addr)){
                registered = false;
            }
        }
        if(!registered && ActuatorStackSize < ACTUATOR_CONTROLLER_COUNT){
            //this is a new actuator and we have enough space in the table, record this
            uip_ipaddr_copy(ActuatorStack + ActuatorStackSize, sender_addr);
            ActuatorStackSize++;

            LOG_INFO("New actuator identified, register into routing table.\n");

            //return a message to the actuator as we have added it to the network
            static char ack[8];
            snprintf(ack, sizeof(ack), SINK_ACK);
            simple_udp_sendto(&toActuator_conn, ack, strlen(ack), sender_addr);
            
        }
    }
}

PROCESS_THREAD(sinkProcess, ev, data){
    PROCESS_BEGIN();

    //clear the memory of actuator routing table
    memset(ActuatorStack, 0x00, sizeof(ActuatorStack));
    ActuatorStackSize = 0ull;

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

    PROCESS_END();
}