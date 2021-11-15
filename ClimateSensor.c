#include "Common.h"

static struct simple_udp_connection unicast_conn;

PROCESS(climateSensorProcess, "Climate Sensor");
AUTOSTART_PROCESSES(&climateSensorProcess);

static void climate_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){
}

PROCESS_THREAD(climateSensorProcess, ev, data){
    static struct etimer sender_delay;
    uip_ipaddr_t addr;
    static char msg[32];

    PROCESS_BEGIN();

    //Initialise UDP connection
    if(!simple_udp_register(&unicast_conn, CLIMATE_SENSOR_PORT, NULL, SINK_PORT, &climate_rx_callback)){
        LOG_ERR("UDP connection for the climate sensor could not be setup.");
        //explicitly kill the process
        PROCESS_EXIT();
    }

    etimer_set(&sender_delay, CLOCK_SECOND * 10);
    while(true){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));
        etimer_reset(&sender_delay);

        //read the temperature from its sensor
        const float temp = getTemperature();

        //attempt to send temperature to sink

    }

    PROCESS_END();
}