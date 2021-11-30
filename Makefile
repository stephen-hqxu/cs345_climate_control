CONTIKI_PROJECT = Relay ClimateSensor Sink ActuatorController

# build targets
all : ${CONTIKI_PROJECT}

PROJECT_SOURCEFILES += Climate.c

CONTIKI = ../..

include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)
#dependencies used
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_NET_DIR)/ipv6/multicast

#switch to RPL classic
MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC

include ${CONTIKI}/Makefile.include
