CONTIKI_PROJECT = Relay ClimateSensor Sink ActuatorController

# build targets
all : ${CONTIKI_PROJECT}

PROJECT_SOURCEFILES += Climate.c

CONTIKI = ../..
include ${CONTIKI}/Makefile.include
