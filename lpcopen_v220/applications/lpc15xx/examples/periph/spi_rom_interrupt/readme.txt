LPC15xx SPI bus master (interrupt) example using the ROM API

Example description:
--------------------
This example shows how to configure SPI as a bus master in interrupt mode using
the ROM-based APIs.
This example transmits an array of uint16_t values (0x1111, 0x2222, 0x3333, 0x4444) as SPI master
using interrupt mode. The SPI is configured to be in loop back mode and the received data is compared
against the transmitted data, if they match the transmission is repeated and the LED 0 keeps flickering
In case of any error or mismatch in received and transmitted data, LED 0 will stay On.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

