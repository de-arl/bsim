# bsim
---
bsim is an agent-based simulation framework for simulating bats in a 3D environment.
A technical report containing some screenshots can be found in the 'doc' directory.
The code is well documented in the source files.

## Installation
See doc/INSTALL.md for installation instructions.
After successful installation you can try the program with the provided example configuration files.

## Configuration files
Example configuration files are provided in the 'config' directory.
If no configuration file is specified, the program starts with the file 
`config/simulation_parameters.toml` and `animation_parameters.toml` respectively.
If theses files are not found, the program will use default values.

## Graphical user interface 
GUI is only available if built with animation support. 
Most options are self explanatory. 

## Resuming a simulation
If a simulation is interrupted, the program will save the current state to a hidden file named '.state_backup.toml' in the current working directory.
This file can be used to resume the simulation by loading it from the GUI (Simulation -> Simulation control -> Simulation state -> Load state from toml file).
Under the same menu, the state can be saved at any time to a file with a name of your choice.

## Command line usage
If the parallel version is compiled, set the OpenMP environment variables with the provided script 'setenv.sh' by running the following command in the executable directory:
> `$ source setenv.sh`  
Run:
> `$ ./bsim -h`  
to see the available options.

## Usage with Graphical User Interface
When you first start the program, the simulation will start with the default parameters.
Click 'Simulation->Animate' to see the scenery.
Then navigate to the 'Simulation->Simulation control' menu and opent the tab 'Simulation control' to start the simulation.

---

Parameters can be loaded and edited under the Parameters Menu.
Make sure to click all apply buttons to apply changes.
The simulation can be controlled in the Simulation control menu.
To apply changes to the parameters, the simulation must be stopped and the 'Init parameters' button must be clicked.
It is under the tab 'Simulation parameters' of the Simulation control menu and only available if the simulation is stopped.
The log level can be set in the Output tab.
Graphical parameters can be changed any time in the View menu and be saved in the Parameters menu.
To move the scenery, drag the mouse while holding the left mouse button. 
If the control key is pressed, the scenery will be rotated around the x and y axis.
To rotate the scenery around the z axis, hold the control key and the middle mouse button.
Zoom in and out with the mouse wheel.
In the Recod menu, you have the option to save every frame as an webp image (Record frames), record a video (Record movie) or create an animated webp image (Record webp).

## Parameters
All parameters should be set in SI units (meters, seconds, etc.).
The 'strategy' parameter decides which strategy the agents will use.
Available strategies can be found in src/State/Agent.cpp (moveStrategy# functions).
New strategies can be added there.
Currently available strategies are:
-0: Do nothing
-1: No communication, only search and approach food
-2: Like 1, but audible food sources are communicated between agents of classes defined as friends
-3: Like 2, but enemies are avoided
-4: See file, not ready yet
-5: Only avoid enemies
-6: Avoid enemies + strategy 1
-7: Brownian motion
-8: Search only, that is perform a random walk based on the 'brownian_search_duration' parameter

## Search strategy
Defined by the brownian_search_duration parameter.
If set to 0, the agent will perform a Levy flight.
If set to a value greater than 0, the agent will perform a brownian search for the given duration in between Levy flights.

## Examples

> `$ ./bsim -g -a my_animation_parameters.toml`
> Run gui mode with given animation config, if compiled with animation.

> `$ ./bsim -c my_simulation_parameters.toml -s 1 -t 1000 -n 3 -d 1 -l 1 -v 1`
> Run simulation in terminal mode with given simulation config, random seed 1, 1000 time steps, 3 runs, time step 1, log level 1, verbose level 1.

## Notes
For small problem sizes parallelization overhead outweighs runtime benefits. 
Therefor make sure you choose an efficient level of parallelism by compiling PAPI and OMP module and running several test with different numbers of processes to find the most efficient number of threads to use. 

## Author

The program can be freely used without any warranty under the terms of the MIT license.
If you have any questions or suggestions, please contact the author.
München 2024, Andreas Reiner Laible  
email: <andreas.laible@cit.tum.de>
