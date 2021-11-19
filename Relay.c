#include "Common.h"

#define LOG_MODULE "Relay"

static struct simple_udp_connection relay_conn;

PROCESS(relayProcess, "Relay");
AUTOSTART_PROCESSES(&relayProcess);

static void relay_rx_callback(struct simple_udp_connection* conn, 
    const uip_ipaddr_t* sender_addr, uint16_t sender_port, 
    const uip_ipaddr_t* receiver_addr, uint16_t receiver_port, 
    const uint8_t* data, uint16_t data_length){

}

PROCESS_THREAD(relayProcess, ev, data){
    static struct etimer sender_delay;
    uip_ipaddr_t addr;
    static char msg[32];

    PROCESS_BEGIN();

    //Initialise UDP connection
    if(!simple_udp_register(&relay_conn, RELAY_PORT, NULL, SINK_PORT, &relay_rx_callback)){
        LOG_ERR("UDP connection for the relay node could not be setup.");
        PROCESS_EXIT();
    }

    etimer_set(&sender_delay, CLOCK_SECOND * 10);
    while(true){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sender_delay));
        etimer_reset(&sender_delay);
    }

    PROCESS_END();
}