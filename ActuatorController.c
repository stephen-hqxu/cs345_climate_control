#include "Common.h"

#define LOG_MODULE "ActuatorController"

static struct simple_udp_connection toSink_conn;

PROCESS(actuatorProcess, "ActuatorController");
AUTOSTART_PROCESSES(&actuatorProcess);

static void actuator_sink_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
    static uip_ipaddr_t sink_addr;

    //let the sensor aware that the AC is on so we can mutate the climate (just for simulation's sake)
    //because our actuator doesn't have the routing information to the sensor, we do it via sink
    const char* msg = (char*)data;
    if(strncmp(msg, ACTUATOR_ON, data_length) == 0){
        //request to turn on AC
        LOG_INFO("Received sink signal to turn actuator ON from ");

    }else if(strncmp(msg, ACTUATOR_OFF, data_length) == 0){
        //request to turn off AC
        LOG_INFO("Received sink signal to turn actuator OFF from ");

    }

    if(NETSTACK_ROUTING.get_root_ipaddr(&sink_addr)){
        LOG_INFO_6ADDR(sender_addr);
        LOG_INFO_("\n");

        simple_udp_sendto(&toSink_conn, data, data_length, sender_addr);

        LOG_INFO("Replied actuator status to ");
        LOG_INFO_6ADDR(&sink_addr);
        LOG_INFO_("\n");

    }else{
        LOG_ERR("Sink cannot be reached.\n");
    }
    
}

/**
 * @brief Join the multicast group belongs to actuator and sink.
 **/
static const uip_ds6_maddr_t* join_actuator_sink_mcastGroup(){
    uip_ipaddr_t addr;
    uip_ds6_maddr_t* rv;
    const uip_ipaddr_t* const prefix = uip_ds6_default_prefix();
    
    //set ipv6 global addr
    uip_ip6addr_copy(&addr, prefix);
    uip_ds6_set_addr_iid(&addr, &uip_lladdr);
    uip_ds6_addr_add(&addr, 0, ADDR_AUTOCONF);

    //multicast group address
    ACTUATOR_SINK_MCAST_GROUP_ADDR(addr);
    rv = uip_ds6_maddr_add(&addr);
    //check joining status
    if(rv != NULL){
        LOG_INFO("Actuator successfully joined the multicast group to sink with address ");
        LOG_INFO_6ADDR(&(uip_ds6_maddr_lookup(&addr)->ipaddr));
        LOG_INFO_("\n");
    }
    return rv;
}

PROCESS_THREAD(actuatorProcess, ev, data){
    PROCESS_BEGIN();

    //Initialise UDP connection
    if(!simple_udp_register(&toSink_conn, ACTUATOR_PORT, NULL, SINK_PORT, &actuator_sink_rx_callback)){
        LOG_ERR("UDP connection from the actuator to the sink could not be setup.");
        PROCESS_EXIT();
    }
    //join the multicast group to sink
    if(join_actuator_sink_mcastGroup() == NULL){
        LOG_ERR("Actuator fails to join the multicast group to the sink.");
        PROCESS_EXIT();
    }

    while(true){
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}