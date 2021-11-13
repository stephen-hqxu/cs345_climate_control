CONTIKI_PROJECT = Application

# build targets
all : ${CONTIKI_PROJECT}

PROJECR_SOURCEFILES += Climate.c Sink.c ActuatorSensor.c ClimateSensor.c

CONTIKI = ../..
inlcude ${CONTIKI}/Makefile.include
