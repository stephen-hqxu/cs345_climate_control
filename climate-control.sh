#!/bin/sh

simulationOutput="./run"

#copy javascript to the COOJA simulation script
prepare_simulation(){
    simulation_identifier="@Simulation.xml@"

    #attempt to run the simulation in a new folder
    mkdir -p $simulationOutput

    #copy COOJA script to the output make sure we don't change it by accident
    cp ./Simulation/climate-control.csc ./$simulationOutput/
    cd ./$simulationOutput/
    #insert line with templates
    sed -i "/${simulation_identifier}/ r ../Simulation/Simulation.xml" ./climate-control.csc
    #remove template identifiers
    sed -i -e "/${simulation_identifier}/ c\ " ./climate-control.csc
}

if test "$1" == "clean"
then
    #clean simulation log
    rm -rf $simulationOutput
    #clean compiled binary
    make distclean
else
    #prepare simulation script
    prepare_simulation

    #run COOJA NoGUI simulation
    java -jar ../../../tools/cooja/dist/cooja.jar -nogui='./climate-control.csc' -contiki-ng='../../..'
fi

exit 0
