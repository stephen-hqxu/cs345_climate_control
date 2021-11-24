#include "Common.h"

#define LOG_MODULE "ActuatorController"

static struct simple_udp_connection toSink_conn;
static struct simple_udp_connection toClimateSensor_conn;
static struct simple_udp_connection discoverClimateSensor_conn;

PROCESS(actuatorProcess, "ActuatorController");
AUTOSTART_PROCESSES(&actuatorProcess);

static bool inNetwork;

static void actuator_sink_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

    //let the sensor aware that the AC is on so we can mutate the climate (just for simulation's sake)

    const char* msg = (char*)data;
    if(strncmp(msg, SINK_ACK, data_length) == 0){
        //sink accepts the presence of this actuator
        inNetwork = true;

        LOG_INFO("Received sink ACK for joining the network from ");

    }else if(strncmp(msg, ACTUATOR_ON, data_length) == 0){
        //request to turn on AC
        LOG_INFO("Received sink signal to turn actuator ON from ");


    }else if(strncmp(msg, ACTUATOR_OFF, data_length) == 0){
        //request to turn off AC
        LOG_INFO("Received sink signal to turn actuator OFF from ");

    }else{
        LOG_WARN("Received unknown opcode '%.*s'\n", data_length, msg);

        return;
    }

    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");
    
}

static void actuator_climate_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

}

static void actuator_climate_broadcast_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

}

PROCESS_THREAD(actuatorProcess, ev, data){
    static struct etimer sender_delay;
    uip_ipaddr_t dest_addr;
    static char msg[32];

    PROCESS_BEGIN();

    //set initial network status
    inNetwork = false;

    //Initialise UDP connection
    if(!simple_udp_register(&toSink_conn, ACTUATOR_PORT, NULL, SINK_PORT, &actuator_sink_rx_callback)){
        LOG_ERR("UDP connection from the actuator to the sink could not be setup.");
        PROCESS_EXIT();
    }
    if(!simple_udp_register(&toClimateSensor_conn, ACTUATOR_PORT, NULL, CLIMATE_SENSOR_PORT, &actuator_climate_rx_callback)){
        LOG_ERR("UDP connection from the actuator to the climate sensor could not be setup.");
        PROCESS_EXIT();
    }
    if(!simple_udp_register(&discoverClimateSensor_conn, ACTUATOR_CLIMATE_BROADCAST_PORT, NULL, ACTUATOR_CLIMATE_BROADCAST_PORT, &actuator_climate_broadcast_callback)){
        LOG_ERR("UDP connection for actuator to discover climate sensors could not be setup.");
        PROCESS_EXIT();
    }

    //To check regularly if the sink is alive
    //Initially actuator needs to join the network, and it signals the sink regularly.
    etimer_set(&sender_delay, CLOCK_SECOND * 30);
    while(true){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));

        if(NETSTACK_ROUTING.get_root_ipaddr(&dest_addr)){
            snprintf(msg, sizeof(msg), ACTUATOR_STATUS_REQUEST);

            LOG_INFO("Sending actuator status to ");
            LOG_INFO_6ADDR(&dest_addr);
            LOG_INFO_("\n");

            simple_udp_sendto(&toSink_conn, msg, strlen(msg), &dest_addr);

        }else{
            LOG_ERR("Sink cannot be reached.\n");
        }
        
        //if the actuator is in a network, only check network status occasionally
        //otherwise keep sending regular message to search for a network
        etimer_set(&sender_delay, CLOCK_SECOND * 60 * (inNetwork ? 20 : 0.5f));
    }

    PROCESS_END();
}