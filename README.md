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

# 04/13/2025: Milestone #2

This is the socket definition for the robot assignment. It will allow our machines to communicate with the robot and the webserver using either UDP or TCP. 

## Execution Instructions 

Simulation Test:
1) Ensure you have the latest robot simulator application downloaded (the April 2025 version). 
2) Ensure your simulator is set to use port 25543.  

Unit Tests: 
The unit tests have been reorganized by milestone. The milestone #1 tests have been refined for better coverage and accuracy. You should not need to adjust the configuration properties, as we have linked the necessary files directly in the project. 

1) Open 'Test Explorer.'
2) Run all tests. 
3) All tests from ROBOT4_TEST_MILESTONE1 and ROBOT4_TEST_MILESTONE2 should run and all should pass. 

## RESTful Interface
The interface is currently in development with plans to be complete before the demo day: 04/17/2025. Right now, it features a simple webserver interface that allows users to input commands and settings. NOTE: this version is not yet fully functional, however if you'd like to view the current state of the interface, you will need to run it inside a Docker container: 

1) Ensure your machine has Docker running. 
2) From the command terminal, you may run a command similar to: 
`docker run --rm -ti -p 25543:25543 -v C:\Users\ddlou\OneDrive\Documents\GitHub\Robot-4-Team-Project\Robot4\RobotGUI:/SharedDocker webserver_dev bash`
*This may need to change based on the file path on your computer, this is just an example* 
3) Navigate into the SharedDocker directory, then the build directory. 
4) Run the command "make."
5) Run the command "./robot"
6) In your browser, vist "http://localhost:25543/" (*the port may vary if you used a different one in step 2*). 
7) You should see the Robot Control Panel design concept as of 04/13/2025. 

# 04/16/2025: Milestone #3

This is the final version of the robot assignment. Our program can successfully send commands and receive telemetry responses from the robot simulator. It is ready to be tested with the actual robot on 04/17/2025 for the final competition. 

### NOTE
- For the previous milestones, the PktDef and MySocket were written using the WinSock library to allow for unit testing with Visual Studio Native Test. To ensure that the program can be run in a Linux environment with Docker, we have changed the implementation to follow the standards for C++ socket programming in Linux. The core logic and all files remain the same, but we have adjusted the syntax for a Linux environment. 
- `Robot_Group4_Final` will be used in the demo and it is our final version.

## Execution Instructions 

Robot Simulator and Final Demo: 
1) Ensure you have the latest robot simulator application downloaded (the April 2025 version), if using the simulator app. 
2) Set the simulator port to any port you wish to use, if using the simulator app.
3) Run the robot project in a Docker container, using port 25543. 
4) Access the Command GUI from a browser by entering the IP address of the machine running the web server, in the search bar. 
5) Use the GUI to set up your connection by entering the IP and port used by the robot/simulator. 
6) Use the GUI to input command parameters. 
7) "Send" the command to the robot. 

The frontend (Command GUI) handles user input, allowing you to customize the connection setup and robot commands. The Command GUI collaborates with the RESTful routes (in main.cpp) that facilitate the communication between our project and the robot. The input is passed to the backend, where PktDef and MySocket handle the logic for data communication. PktDef will handle serialization/deserialization of data and MySocket will handle the protocols and socket configurations. All feedback from the robot will be displayed on the Command GUI and in the robot/simulator terminal. 
