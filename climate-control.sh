#!/bin/sh

simulationDir="./run"

if test "$1" == "clean"
then
    #clean simulation log
    rm -rf $simulationDir
    #clean compiled binary
    make distclean
else
    #attempt to run the simulation in a new folder
    mkdir -p $simulationDir
    cd $simulationDir

    #run COOJA NoGUI simulation
    java -mx2048m -jar ../../../tools/cooja/dist/cooja.jar -nogui=.'./climate-control.csc' -contiki-ng='../../..'
fi

exit 0
