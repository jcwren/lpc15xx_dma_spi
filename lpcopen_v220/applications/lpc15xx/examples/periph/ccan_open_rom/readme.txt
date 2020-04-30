LPC15xx CAN Open example

Example description:
--------------------
This example will demonstrate sending and receiving of data using CAN open
stack (in ROM). With proper termination and no valid ACK response, the
example code will transmit the 'TEST' packet, wait for 15 bit time periods
and re-transmit the 'TEST' packet.  This will continue until a node responds
with a valid ACK.

Special connection requirements:
--------------------------------
When using the LPC 1549 as a CAN device, it is vital that the transciever 
circuitry (external to the LPC 1549) be properly terminated (i.e 120 ohms 
at both ends of the bus).

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

