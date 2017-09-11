The files here implement a set of simulated aircraft instruments.  Mechanically the instruments are
built from 3D printed plastic parts with various hardware.  The instrument indicators are actuated
via servos and/or stepper motors.  A PCB in each instrument contains a microcontroller (Atmel ATmega168)
which receives messages via a USB port and generates control signals to the actuators.

The overall structure of the simulation consists of the open source flight simulator FlightGear running
on a PC/Laptop, a cockpit controller computer running Linux which runs the CockpitController program,
and the instruments.

The CockpitController program receives simulation state information via UDP.  It processes these
messages and when necessary generates command messages via USB to the various instruments.  Instruments
can be added or removed any time during the simulation, and any number of instruments are handled
including multiple of the same type.

The CockpitIndicatorFirmware runs on the microcontroller in each instrument.  The firmware is
built using avr-gcc.  The firmware was developed using the test framework cpputest-3.6.  To build the
firmware for the target instruments the following sequence is used:

cd <root>/CockpitCode/CockpitIndicatorFirmware/firmware
make -f ../makefirmware.mk TARGET=<target node>

To program the node:

make -f ../makefirmware.mk TARGET=<target node> program

On a newly commissioned instrument PCB, the microcontroller must be configured using:
make -f ../makefirmware.mk TARGET=<target node> pflags

The <target node> may be any of the following:
NodeASI, NodeBPI, NodeCAI, NodeCYL, NOdeFAT, NodeFLA, NodeFLF, NodeFLR, NodeFPI, NodeHPI, NodeMPI,
NodeOPI, NodeOTI, NodeQDR, NodeRPM, NodeSUI, NodeVSI

NodeASI - Air Speed Indicator
NodeBPI - Brake Pressure Indicator
NodeCAI - Carb. Air Temperature Indicator
NodeCYL - Cylinder Head Temperature Indicator
NOdeFAT - Free Air Temperature Indicator
NodeFLA - Fuel Level Aux Tanks
NodeFLF - Fuel Level Front Tanks
NodeFLR - Fuel Level Rear Tanks
NodeFPI - Fuel Pressure Indicator
NodeHPI - Hydraulic Pressure Indicator
NodeMPI - Manifold Pressure Indicator
NodeOPI - Oil Pressure Indicator
NodeOTI - Oil Temperature Indicator
NodeQDR - Throttle Quadrant Monitor
NodeRPM - Engine RPM Indicator
NodeSUI - Suction Indicator
NodeVSI - Vertical Speed Indicator

The directory PCB_Servo contains the PCB proof artwork for the instrument PCB.
