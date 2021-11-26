CONTIKI_PROJECT = Relay ClimateSensor Sink ActuatorController

# build targets
all : ${CONTIKI_PROJECT}

PROJECT_SOURCEFILES += Climate.c

CONTIKI = ../..

include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)
#dependencies used
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_NET_DIR)/ipv6/multicast os/services/simple-energest

include ${CONTIKI}/Makefile.include
