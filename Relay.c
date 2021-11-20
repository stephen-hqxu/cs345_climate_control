#include "Common.h"

#define LOG_MODULE "Relay"

//static struct simple_udp_connection relay_conn;

PROCESS(relayProcess, "Relay");
AUTOSTART_PROCESSES(&relayProcess);

PROCESS_THREAD(relayProcess, ev, data){
    PROCESS_BEGIN();


    PROCESS_END();
}