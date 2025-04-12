# Robot 4: Team Project 
## Destiny Louangsombath & Tyler Dao 

# 04/06/2025: Milestone #1

This is the packet definition for the robot assignment. It allows for data communication between a windows machine and a robot simulator application. 

## Execution Instructions

Simulation Test:
1) Run the robot simulator application. 
2) Run the Main.cpp file in the C++ source code. 
This simulation creates a packet with the DRIVE command with hardcoded values for direction, duration and speed first, after that it sends another SLEEP command. It demonstrates how a packet will be sent to the robot. (NOTE: this simulation is for debugging and testing purposes only. We may make changes in later stages depending on the project's progress.)

Unit Tests: 
1) Go to `Configurations Properties -> Linker -> Input -> Additional Dependencies`, add `PktDef.obj` file in your directory
2) Go to `Configurations Properties -> Linker -> General -> Additional Library Directories`, add `Debug` folder in `Robot4` project
3) Go to `Configurations Properties -> VC++ Directories -> Include Directories`, add `x64` folder in `Robot4` project
4) Run all tests from the ROBOT4_TESTS Native Unit Test Suite. 
These tests ensure the core functionality of the packet definition work as required. More tests may be added later, as necessary. 
