LPC15XX USB to UART bridge example

Example description:
--------------------
The example shows how to us USBD ROM stack to creates a USB to UART converter port.
The example uses UARTN api. Note, LPC15XX has two types of UARTs.

Special connection requirements:
--------------------------------
Connect the USB cable between micro connector on board and to a host.
When connected to Windows host use the .inf included in the project
directory to install the driver. For OSx (Mac) host, no drivers are needed.
When using  @ref LPCOPEN_15XX_BOARD_LPCXPRESSO_1549 boards with a base board you need to remove jumpers 
J54 & J55 on base board and power the system through USB port connected to FTDI chip. Note,
serial port settings such as baud rate, parity and data bits should be same for FTDI port and 
"LPC VCOM port".


Build procedures:
-----------------
Visit the LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

