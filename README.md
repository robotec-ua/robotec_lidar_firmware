# Open-source software for Robotec LiDAR sensor
The project is the software, used in Robotec LiDAR. It is dedicated to control a sensor and a motor, communicate with a master device using different protocols (RS485, UART, SPI, I2C). 

## Table of contents
* [Device](#device)
	* [Specifications](#specifications)
	* [Schematic](#schematic)
	* [Datasheet](#datasheet)
* [Guide](#guide)
	* [Compilation](#compilation)
	* [Installation](#installation)
* [Project Structure](#project_structure)
	* [Folders description](#folders)
	* [Files description](#files)

## Device
TODO
### Specifications
TODO

### Schematic
TODO

### Datasheet
TODO

## Guide
The guide will help you build and use the project in your Robotec LiDAR device.
### Compilation
To build the project and get the SW for the STM32F1XX run the following :
```bash
$ make
```
All the built files will be inside of the `build` folder. 

### Installation
To install the SW use the file `build/robotec_lidar_firmware.bin` and the following tools:
* `ST-LINK Utility` : [link](https://www.st.com/en/development-tools/st-link-v2.html)
* `stm32flash` : [link](https://sourceforge.net/projects/stm32flash/)
* `dfu-utils` : [link](http://dfu-util.sourceforge.net/)

## Project Structure
```
.
├── include
│   ├── commands.h
│   ├── lidar.h
│   ├── main.h
│   ├── motor.h
│   ├── responses.h
│   ├── stm32f1xx_hal_conf.h
│   ├── stm32f1xx_it.h
│   └── tfmini.h
├── lib
├── src
│   ├── lidar.c
│   ├── main.c
│   ├── motor.c
│   ├── startup_stm32f103xb.s
│   ├── stm32f1xx_hal_msp.c
│   ├── stm32f1xx_it.c
│   ├── system_stm32f1xx.c
│   └── tfmini.c
├── tools
│   └── STM32F103C8Tx_FLASH.ld
├── Makefile
└── README.md
```
### Folders
* `include` : project headers
* `lib` : STM32F1XX headers
* `src` : source code

### Files
* `commands.h` : set of Robotec LiDAR predefined commands
* `responses.h` : set of Robotec LiDAR predefined responses
* `lidar.*` : device-specific functions
* `main.*` : the main block of the SW
* `motor.*` : contol the motor using PWM
* `tfmini.*` : communication with the ToF sensor
* `stm32f1xx*` : STM32-specific code
* `STM32F103C8Tx_FLASH.ld` : linker script for the STM32F1XX