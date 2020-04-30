LPC15xx I2C bus master (interrupt) example using the ROM API

Example description:
--------------------
This example shows how to configure I2C as a bus master in interrupt mode using
the ROM-based APIs.
After I2C is setup, the I2C master receive and transmit
functions are called through the built-in ROM routines.
For the LPCXpresso Motor Control Board rev B, this example reads the LM75A temperature sensor on the
base board. The raw value read from LM75A will be printed on UART. This example initializes the OLCD
and fills the screen with white.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

