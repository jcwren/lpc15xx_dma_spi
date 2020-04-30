LPC15xx UART using DMA ROM example

Example description:
--------------------
This examples shows how to use the DMA ROM driver to handle servicing
of UART transmit and receive operations to offload the MCU and reduce
interrupt processing.
UART transmit operations have a known transmit size and are queued to the
DMA controller via descriptors setup for the transfer. Descriptors are
only queued to the DMA controller when data is waiting to be sent.
This specific implementation can queue multiple UART buffers with a single
DMA interrupt per UART buffer sent. This example doesn't used linked
DMA descriptors.
UART receive operations have a fixed list of descriptors and buffer sizes
setup for a Ping Pong transfer, linked list is not supported by the ROM driver.
The DMA controller will generate a DMA interrupt for UART receive only when
a full buffer of data has been received. Once this buffer is received, it
is sent back out via UART transmit via DMA.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

