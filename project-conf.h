#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#include <net/ipv6/multicast/uip-mcast6-engines.h>

/*---------------------------------------------------------------------------*/

//enable energest module
#define ENERGEST_CONF_ON 1

//multicast algorithms
#define UIP_MCAST6_CONF_ENGINE UIP_MCAST6_ENGINE_ESMRF

/*---------------------------------------------------------------------------*/

#endif /* PROJECT_CONF_H_ */