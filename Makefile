CONTIKI_PROJECT = ClimateSensor
#Sink ActuatorSensor

# build targets
all : ${CONTIKI_PROJECT}

PROJECT_SOURCEFILES += Climate.c

CONTIKI = ../..
include ${CONTIKI}/Makefile.include
