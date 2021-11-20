#include "Common.h"

#define LOG_MODULE "ActuatorController"

static struct simple_udp_connection actuator_conn;

PROCESS(actuatorProcess, "ActuatorController");
AUTOSTART_PROCESSES(&actuatorProcess);

static bool inNetwork;

static void actuator_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

    const char* msg = (char*)data;
    if(strncmp(msg, SINK_ACK, data_length) == 0){
        //sink accepts the presence of this actuator
        inNetwork = true;

        LOG_INFO("Received sink ACK for joining the network.\n");

    }else{
        LOG_WARN("Received unknown message: '%.*s'\n", data_length, (char*)data);
    }
    
}

PROCESS_THREAD(actuatorProcess, ev, data){
    static struct etimer sender_delay;
    uip_ipaddr_t dest_addr;
    static char msg[32];

    PROCESS_BEGIN();

    //set initial network status
    inNetwork = false;

    //Initialise UDP connection
    if(!simple_udp_register(&actuator_conn, ACTUATOR_PORT, NULL, SINK_PORT, &actuator_rx_callback)){
        LOG_ERR("UDP connection for the actuator could not be setup.");
        PROCESS_EXIT();
    }

    //To check regularly if the sink is alive
    etimer_set(&sender_delay, CLOCK_SECOND * 20);
    while(true){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));

        if(NETSTACK_ROUTING.get_root_ipaddr(&dest_addr)){
            snprintf(msg, sizeof(msg), ACTUATOR_STATUS_REQUEST);

            LOG_INFO("Sending actuator status request to ");
            LOG_INFO_6ADDR(&dest_addr);
            LOG_INFO_("\n");

            simple_udp_sendto(&actuator_conn, msg, strlen(msg), &dest_addr);

        }else{
            LOG_ERR("Sink cannot be reached.\n");
        }
        
        //if the actuator is in a network, only check network status occasionally
        //otherwise send regular message to search for a network
        etimer_set(&sender_delay, CLOCK_SECOND * (inNetwork ? 60 * 2 : 20));
    }

    PROCESS_END();
}