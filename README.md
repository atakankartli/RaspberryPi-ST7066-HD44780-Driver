# ST7066, HD44780 Driver for Raspberry Pi

This is a repository that provides a driver for the 20x2 ST7066u which is based on HD44780 LCD controller. The driver utilizes the pigpio library as a dependency.

## Installation

To use this driver, you will need to install the pigpio library on your Raspberry Pi. You can do this by running the following command:

```shell
sudo apt install pigpio
```
# Building the Driver

To build the driver, you can use the gcc compiler. Run the following command:

```shell
gcc -c ST7066.c -o ST7066.o -lpigpio
```

This command compiles the ST7066.c file and produces an object file named ST7066.o. The -lpigpio flag links the pigpio library during the compilation process.

After building the driver, you can incorporate it into your project and utilize the functionalities provided by the ST7066 and HD44780 LCD controllers.

# Usage
Include the ST7066.h header file in your code, call "gpioInitialise()" for pigpio and "Startup()" for screen initialization and make use of the driver functions provided to interact with the ST7066 or HD44780 LCD controller.

```shell
#include "ST7066.h"

gpioInitialise(); 
Startup();
// Your code here
```

# License
This project is licensed under the MIT License. Feel free to use and modify the code according to your needs.
