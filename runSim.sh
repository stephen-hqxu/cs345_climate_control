#!/bin/sh

#attempt to run the simulation in a new folder
mkdir -p ./run
cd ./run

#run COOJA
java -mx2048m -jar ../../../tools/cooja/dist/cooja.jar -nogui=.'./climate-control.csc' -contiki-ng='../../..'

exit 0