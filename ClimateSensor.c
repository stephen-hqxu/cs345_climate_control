#include "Common.h"

#define LOG_MODULE "ClimateSensor"

static struct simple_udp_connection toSink_conn;

PROCESS(climateSensorProcess, "Climate Sensor");
AUTOSTART_PROCESSES(&climateSensorProcess);

static void climate_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

    const char* msg = (char*)data;
    //update AC status
    if(strncmp(msg, ACTUATOR_ON, data_length) == 0){
        setActuatorMotor(true);

    }else if(strncmp(msg, ACTUATOR_OFF, data_length) == 0){
        setActuatorMotor(false);
    }
}

/**
 * @brief Join the multicast group from sink to climate sensor
 **/
static const uip_ds6_maddr_t* join_sink_climate_mcastGroup(){
    uip_ipaddr_t addr;
    uip_ds6_maddr_t* rv;
    const uip_ipaddr_t* const prefix = uip_ds6_default_prefix();
    
    uip_ip6addr_copy(&addr, prefix);
    uip_ds6_set_addr_iid(&addr, &uip_lladdr);
    uip_ds6_addr_add(&addr, 0, ADDR_AUTOCONF);

    SINK_CLIMATE_SENSOR_MCAST_GROUP_ADDR(addr);
    rv = uip_ds6_maddr_add(&addr);
    if(rv != NULL){
        LOG_INFO("Climate sensor successfully joined the multicast group to sink with address ");
        LOG_INFO_6ADDR(&(uip_ds6_maddr_lookup(&addr)->ipaddr));
        LOG_INFO_("\n");
    }
    return rv;
}

PROCESS_THREAD(climateSensorProcess, ev, data){
    static struct etimer sender_delay;
    uip_ipaddr_t dest_addr;
    static char msg[32];

    PROCESS_BEGIN();

    //Initialise UDP connection
    if(!simple_udp_register(&toSink_conn, CLIMATE_SENSOR_PORT, NULL, SINK_PORT, &climate_rx_callback)){
        LOG_ERR("UDP connection for the climate sensor could not be setup.\n");
        //explicitly kill the process
        PROCESS_EXIT();
    }
    //join multicast group to sink
    if(join_sink_climate_mcastGroup() == NULL){
        LOG_ERR("Climate sensor fails to join the multicast group to the sink.");
        PROCESS_EXIT();
    }

    //initialise the climate environment held by the sensor
    initClimate();

    //Every 5 mins, the temperature sensors send a message to the sink.
    etimer_set(&sender_delay, CLOCK_SECOND * 60 * 5);
    while(true){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));

        //read the temperature from its sensor
        const float temp = getTemperature();

        if(NETSTACK_ROUTING.get_root_ipaddr(&dest_addr)){
            //attempt to send temperature to sink
            snprintf(msg, sizeof(msg), "%f", temp);

            LOG_INFO("Sending sensor reading '%f' to ", temp);
            LOG_INFO_6ADDR(&dest_addr);
            LOG_INFO_("\n");

            simple_udp_sendto(&toSink_conn, msg, strlen(msg), &dest_addr);

        }else{
            LOG_ERR("Sink cannot be reached.\n");
        }

        etimer_reset(&sender_delay);
    }

    PROCESS_END();
}