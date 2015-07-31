# fhvOS

This is a monolithic operating system for the Beagle Bone White Rev. A6a.

Documentation
------------

The documentation of this project can be found [here](/documentation/Documentation.pdf).


Repository structure
------------

The repository is structured as follows:

1. `api` contains the *CCS* project for the system API
2. `applications` a bunch of demo user applications as *CCS* projects for the operating system
3. `docs` contains useful information about the *ARM* architecture and *TI* compiler
4. `documentation` contains the *LaTeX* documentation for the project
5. `os` contains the *CCS* project of the kernel implementation (this is the actual OS)
6. `presentations` contains presentations/slides about the OS
7. `projectmanagement` contains *MS Project* project which shows the time schedule of the implementation tasks
8. `prototyping` contains some *CCS* projects for testing purposes


Build instructions
------------

To successfully build and deploy the OS on your *Beaglebone* you should follow these steps:

1. Import the *CCS* project which is contained in the `os` folder (don't build yet)
2. Import the *CCS* project which is contained in the `api` folder
3. Build both targets, i.e. *Full* and *Light*, of the `api` project 
4. Build the `os` project

Alternatively, you may import the demo user applications into your *CCS* workspace. This step should be straightforward.
