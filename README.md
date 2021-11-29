# cs345_climate_control

Year 3 Sensor Networks and Mobile Data Communications Coursework

## Build instructions

- Make sure you unzip the project archive under Contiki root directory. Once you have done that *coursework* folder should appear.
- Go to the climate control main application by:

```sh

cd ./coursework/climate-control

```

> In the Makefile the project locates Contiki root using `../..`.

- To start automated No-GUI simulation, in the console:

```sh

./climate-control.sh

```

The simulation is set to run for 15000 seconds (around 4.2 hours), and it shows the actuators turn on and off twice each. The simulation script is written in *Javascript* and parsed using *Java Nashorn* engine.

> There is some misinformation in Cotiki wiki page where it says the script uses *Java Rhino* engine, in fact it is not the case, tested on DCS machine with OpenJDK-11.0.13.

- To delete every compiled binaries, type:

```sh

./climate-control.sh clean

```

- To start simulation using COOJA GUI, you can find the simulation under *Simulation* directory. Open *climate-control.csc* using COOJA and start the simulation.
- Under *Visualisation* folder it contains a python script for parsing logs and plotting diagrams. Outputs are visualised using *Jupyter notebook*. Output images are all the same as shown in the report.