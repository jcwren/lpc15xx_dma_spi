LPC15xx Pin Interrupt example

Example description:
--------------------
The Pin interrupt example demonstrates using the pin interrupt API functions.
This example configures a pin interrupt as a falling edge interrupt.
The GPIO input PIO0_17 (WAKEUP/SW1 switch on LPCXpresso LPC1549--use a jumper
wire on the KEIL MCB1500 board) generates the interrupt signal.
The application will spin in a loop.  With every pin interrupt the 
board LED0 will be toggled.

Special connection requirements:
--------------------------------
The GPIO input PIO0_17 (WAKEUP/SW1 switch on LPCXpresso LPC1549--use a jumper
wire on the KEIL MCB1500 board) generates the interrupt signal.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

