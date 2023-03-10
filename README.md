# Serial Peripheral Interface (SPI) Memory Reader

## Description

**Summary**: An application written in C for serial communication with a peripheral device on bit-level. Its goal is to read data from a peripheral storage device, correct transmission errors, and write the data to an output.

In this project, I demonstrate skills in
* programming in C, 
* working with hardware devices on a low-level, 
* maintaining balance between a reusable design and efficient code.

TODO Add some introductory description here
- Output File + Log File
- SPI serial interface
- Error correction via parity bits
- A given set of memory addresses

## Table of Contents 

- [Problem Definition](#problem-definition)
- [Installation](#installation)
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

## Problem Definition

TODO ~15min: Define the problem using SSRL's outline

## Installation
System Requirements:
- GCC compiler installed
- Make installed

The project's compilation and installing is managed with Make. To run the application on your machine, follow these steps: 
1. Clone the repository
2. 
    Open a terminal, switch your working directory to the repositories root, execute the command `make`:    
    ```
    $ make
    ```
    This will make use of the provided `Makefile` in the repositories root folder to compile and link the projects source files into an executable on your system. The executable is generated in the path `bin/read_cots_memory_via_spi_app`.


## Usage

For demonstartion reasons, the project comes with an example implementation of the peripheral device API. It is provided through a pre-compiled c object file which can be found in `lib/component.o`. 

To use this project with your own peripheral device, you need to provide an implementation of the API interface in `src/component.h`. This interface is further explained in the [Problem Definition](#problem-definition).

To use the application with the provided API implementation, follow the steps in [Installation](#installation) and execute the generated executable `bin/read_cots_memory_via_spi_app` on the command-line.

The application will start and prompt for a frequency to interact with the peripheral device and name of an output file. 
- The provided frequency is the frequency the application will use for serial communication with the peripheral device. More precicsely, it defines the clock frequency to use for the Serial Peripheral Interface (SPI) protocol.

- The provided file specifies a path to where the the application should write the read memory content. 


<img src="assets/images/readme_usage_frequency_and_output_file_prompt.png" alt="Application prompts for frequency and file name" style="width:400px;"/>

After the user entered the required inputs, the application will confirm the inputs, read the memory from the peripheral device and return as soon as it is done reading.

<img src="assets/images/readme_usage_application_returned.png" alt="Application returns after it is done" style="width:600px;"/>

## Credits

This project was made as a demonstartion of skills in C programming, embedded systems, and low-level hardware interaction in cooperation with the University of Georgia's Small Satellite Research Laboratory ([SSRL]((https://www.smallsat.uga.edu/))). The API for the peripheral component was created by them.

## License

Licensed under the [GNU General Public License](LICENSE).

---


























# UGA SSRL MOCI Flight Software Spring 2023 Take Home Project
Congratulations on passing your first round of interviews for the UGA SSRL! Your Round 2 interview will be in the form of the Flight Software take-home project outlined in this document. If you have any questions in regards to the instructions or even if you need help or clarification with any part of the project, I highly encourage you to contact me at jcl71292@uga.edu. The projects will be scored based not only on whether project criteria are met, but also whether or not it is obvious that you put in effort into your submission. Don't fret if you can't get something working, just do your best work!

# Project Outline
Flight Software on MOCI (or any satellite) often involves communicating data between many separate components in an efficient and reliable way. Your take-home project will consist of a program written in C which handles packet communication between a COTS (Commercial Off-the-Shelf) component and the component which will be running your program. An API will be provided to you that you can use to communicate with the component, which will send you raw data with a specified packet format. Your task is to decode this data into useful, human readable format while also handling any necessary error correction on the data.

Your program will communicate with the component via SPI (Serial Peripheral Interface). If you are already familiar with SPI, great! If not, no need to worry, all the necessary information to communicate with the component will be provided. Essentially, your program will generate a clock pulse which switches back and forth between high and low. In addition to this clock line, there will be two data lines for communication. One from your program into the component (Master Out Slave In, or MOSI) and one from the component into your program (Master In Slave Out, MISO). On each rising edge of the clock line, the component will sample the level of the MOSI line, and if any data is to be sent back, the MISO line will also switch level on the rising clock edge. A timing diagram for this commnication is provided below, with the dotted line indicating when a bit is sampled or set:
![SPI_Diagram](https://user-images.githubusercontent.com/60021392/216702495-3152ef46-6e03-4b49-bb20-472a00e10fbf.png)
In order to actually implement this communication protocol in code, the component API provided in this repository has 3 functions you can use.
* void SET_CLK(boolean level) - Sets the clock line to the specified boolean logic level.
* void SET_MOSI(boolean level) - Sets the MOSI line to the specified boolean logic level.
* boolean READ_MISO(); - Returns the boolean logic level of the MISO line.

Now that we understand WHEN to send bits, we will look at WHAT bits to send. In this particular application, we are only concerned with reading data from the device, although in many other applications, SPI communication can be much more complex. Although our component is essentially a black box (we do not understand, nor do we need to be concerned with, the internal workings) we do know that its addressable memory is comprised of 256 8-bit words. Every communication between the devices will be initiated by the host (your program). In order to read data from the component, the program must send 2 bytes to the component to receive a single byte of data from a particular memory address of the component. 16 clock pulses MUST be completed for the component to begin relaying data, and an additional 8 clock pulses, for a total of 24, MUST be completed to complete the communication with the device. Shown below is a packet format diagram:
![PacketFormat](https://user-images.githubusercontent.com/60021392/216746401-09227e95-4c17-4afc-b1d1-edd6eccb787d.png)
As far as your program is concerned, the "read" command byte will always be the same - 0x55. The data byte will consist of 7 data bits, with the least significant bit acting as an even parity bit. You can read more on parity bits [here](https://www.tutorialspoint.com/what-is-a-parity-bit). The program will use this parity bit to check the data it receives for errors. For the purposes of this project, you can always assume that any error in the data resulted in an odd number of bits flipping. The 7 bits of data will be encoded in 7 bit ASCII. 


The program should be able to:
* Read data from every address on the component
* Detect and correct errors in the data by re-requesting the data
* Print the contents of the component memory to a human readable plain-text file


# Getting Started
Everything you should need (except for your own code, of course!) is included in this repository. You should clone it locally and work from that copy.

You can use the commands outlined above to interact with the **component.o** file. Be sure to create a makefile for compilation. The user should be able to run ">make clean" followed by ">make" in the terminal, and have your program compile into an executable file.

# Submission
You should submit a zip file of all the necessary files to compile and run your program to jcl71292@uga.edu by Tuesday, February 14th at 11:59 PM. Once again, if you have any questions at all, do not hesitate to reach out to me!

Good luck!
