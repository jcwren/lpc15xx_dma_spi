LPC15xx DMA memory to memory transfer example

Example description:
--------------------
This example shows how to setup and use the DMA controller for a limited
software triggered memory to memory transfer. Because of the limited burst
transfer size of the DMA (1024 transfers max) and limited burst size, DMA
M2M operations are somewhat specialized, but they do have a good performance
boost over memcpy() and memmove() functions. They would work best for
smaller transfers.
Time measurement for performance is in milliSeconds. Lower values indicate
higher data transfer performance. A time 50% smaller than another time would
mean the same operation was about 2x faster. Times are provided for similar
memmove(), memcpy(), and DMA data move operations.
The example also includes a test enable definition (XFERTEST) that will
also perform data testing when it is defined.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

