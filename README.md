Source code in this repository belongs to project focussing on gaze cuing effect in human robot interaction. Code control robot iCub in two forms of embodiment. Code was developed, tested and used under Ubuntu 20.04.4 LTS (Focal Fossa) OS.

Repo contains: 
- SDF model of world with robot iCub used for development with Gazebo simulator
- folder _with-lights_ - which use our custom made lights for creating stimuli. This folder contains source code which demand connecting these ligths via USB.
- folder _without-lights_ - contains source code that can be used without our custom made lights. Stimuli are simulated via console output.
- folder results, which contains data measured by this soft for the gaze cuing experiment.



Usage:

First, we select whether we want to use real hardware lights or only console as stimuli. Based on this we wil choose the folder _with-lights_ or _without-lights_. Then we choose the robot form of embodiment - physical iCub robot (folder physical) or robot in iCub simulator (folder simulator).

If we selected robot in simulator, we need to :
- run YARP server
- run simulator iCubSIM

If we selected physical robot, we need to :
- connect to YARP server, where the robot iCub is connected

Then we have to compile program in selected folder using CMake.
After compilation, program can be run from terminal window with parameter --robot (robots name in YARP server, ie. icubSim when using iCub simulator).

Examples:
./<name of compiled file> --robot <robot name>, 
./simulator --robot icubSim. 
