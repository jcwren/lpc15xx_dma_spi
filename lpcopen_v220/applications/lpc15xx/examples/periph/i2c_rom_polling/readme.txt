LPC15xx I2C bus master (polling) example using the ROM API

Example description:
--------------------
This example shows how to configure I2C as a bus master in polling mode using
the ROM-based APIs.
After I2C is setup, the I2C master receive and transmit
functions are called through the built-in ROM routines.
For the LPCXpresso Motor Control Board rev B, this example reads the LM75A temperature sensor on the
base board and prints the raw value to UART. It also fills the OLCD screen with white color by writing
to the LCD screen using I2C.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

