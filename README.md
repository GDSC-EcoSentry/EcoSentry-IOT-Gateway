# NRF24 ESP8266 Gateway

## Overview

 This is the EcoSentry's gateway that wirelessly collect data throughout the forest and upload it to Firebase via RESTful HTTP APIs. Developed using the powerful PlatformIO toolset, this project aims to provide real-time data insights into forest conditions, enabling rapid response to environmental changes and potential hazards.

## Features

- Wireless data collection using NRF24 modules.
- Data upload to Firebase for real-time analytics.
- Low power consumption for extended field deployment.
- Flexible, scalable architecture to add more nodes as needed.
- Built using PlatformIO for easy development and deployment.

## Mechanism

1. **Data Transmission**: The collected data is received wirelessly through the NRF24 module.
2. **Data Upload**: The central node uploads the received data to Firebase using RESTful HTTP APIs, where it can be accessed and analyzed in real-time.

## Hardware Requirements

- Wifi enabled microcontroller compatible with PlatformIO (ESP8266)
- NRF24L01+ radio modules

## Software Requirements

- PlatformIO IDE or PlatformIO Core CLI tool
- Existing Firestore database.

## Setting Up the Development Environment

To set up your development environment for coding and deploying the forest monitoring node:

1. **Install PlatformIO**: Follow [PlatformIO's installation guide](https://docs.platformio.org/en/latest/core/installation.html) to install the IDE or CLI on your computer.
2. **Firebase Configuration**: Use created Firebase API to communicate your project.
3. **Environment Configuration**: Install dependency (PlatformIO should be able to do this automatically). 

## Building and Deploying Nodes

1. **Assemble the Hardware**: Connect your NRF24 module to the microcontroller according to the pinout provided below
2. **Configure Software**: Set up your Wifi Name and Password in the code.
3. **Upload Code**: Use PlatformIO to compile and upload your code to the microcontroller.
4. **Deploy in Field**: Connect to 5V power source via USB or connected directly to computer to monitor its output and debug.

## Contributing

We welcome contributors to help expand the capabilities of our forest monitoring nodes. If you're interested in contributing, please fork the repository and submit a pull request with your proposed changes.

## Acknowledgments

- The NRF24 library contributors for the wireless communication capabilities.
- The Firebase team for providing a scalable real-time database.
- The PlatformIO team for their excellent development tools.



## Hardware pinout
| NRF24 | ESP8266 12E |
|-------|-------------|
| GND   | GND         |
| VCC   | 3.3V        |
| CE    | D0          |
| CSN   | D8          |
| SCK   | D5          |
| MOSI  | D7          |
| MISO  | D6          |
| IRQ   | D1          |

