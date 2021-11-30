#!/bin/sh

simulationOutput="./run"

if test "$1" == "clean"
then
    #clean simulation log
    rm -rf $simulationOutput
    #clean compiled binary
    make distclean
else
    #attempt to run the simulation in a new folder
    mkdir -p $simulationOutput
    cd $simulationOutput

    #run COOJA NoGUI simulation
    java -mx2048m -jar ../../../tools/cooja/dist/cooja.jar -nogui='../Simulation/climate-control.csc' -contiki-ng='../../..'
fi

exit 0
