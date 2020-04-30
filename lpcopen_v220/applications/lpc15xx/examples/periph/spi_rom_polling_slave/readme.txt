LPC15xx SPI bus slave (polling) example using the ROM API

Example description:
--------------------
This example shows how to configure SPI as a bus slave in polling mode using
the ROM-based APIs.
This example configures SPI as a slave to read an array of 4 words of 16 bits using polling mode.
The received data is output through the Debug UART port.
To test the example a SPI master needs to be connected, the SPI master needs to keep transmitting
the 4 words which will be outputted on the UART port of the slave. Since the slave transmits the received
SPI data on UART, the master should allow sufficient (around 10ms) delay between each transfer. 
* 
Special connection requirements:
--------------------------------
A SPI master should be connected to test this example the SPI connections are as below: 
SCK - P0.0 - J2 pin7 on Expresso board 
MOSI - P0.16 - J2 pin5 on Expresso board 
MISO - P0.10 - J2 pin4 on Expresso board 
SSEL0 - P0.9 - J2 pin3 on Expresso board 

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

