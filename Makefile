CONTIKI_PROJECT = ClimateSensor Sink
#Relay ActuatorSensor

# build targets
all : ${CONTIKI_PROJECT}

PROJECT_SOURCEFILES += Climate.c

CONTIKI = ../..
include ${CONTIKI}/Makefile.include
