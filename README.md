# RSLK_Example

This example demonstrates a simple state machine that can be programmed to control the robot.  The state machine runs in a continuous loop and samples both the light sensors and bump sensors at the top of the loop.  Different states can be created that represent different stages of the program.  The next state can be transistioned to by setting the 'state' variable.

Currently, switch S1 starts the program and switch S2 is an emergency halt switch.

This example uses the left and right motor encoders.  To connect the encoders, four jumper wires will need to be placed between the two encoder headers (2-pin 45 degree bent pins) and the red MSP432 board.
Use female to male jumpers to connect:

- Left Encoder A connected to P6.4 (J1)                     .
- Left Encoder B connected to P6.5 (J1)
- Right Encoder A connected to P5.4 (J3)
- Right Encoder B connected to P5.5 (J3)

---
## Installing Code Composer Simplelink module

1. Select View -> Resource Explorer
2. Select Folder Software -> **SimpleLink MSP432P4 SDK -v:2.30.00.14**
3. Select the icon: **Download and Install**
