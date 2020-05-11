# LPC15xx SPI DMA demo code

## Overview

This code demonstrates how to use the DMA controller with SPI on the LPC15xx
processor (specifically the LPC1549). While it's targeted for the OM13056
Xpresso LPC1549 board, it only uses the LEDs. The SPI port is routed to the J1
connector, using the same pins the Arduino would.

The demo implements a circular buffer that DMA requests are stored into. This
allows code to queue up several requests without waiting for the first one to
complete. If a request is made and the buffer is full, then a wait loop is run
until an entry is free (this uses the FreeRTOS `vTaskDelay()` call, which will
yield to other tasks).

As each DMA requests completes, if there are additional entries in the circular
buffer, the DMA interrupt handler will start the next entry.

If the length of the DMA request is <= 1024 words (where a word is the size of
each entry, which can be 1, 2, or 4 bytes), the `dmaSPI()` function can be
used.  If the request is > 1024 words, then the `dmaSPIEx()` function can be
used, which will fragment the requests into blocks of 1024 words.

The demo will send 26 strings of 32 characters to the SPI0 port, delay 1
second, and then repeat forever. The red LED (PIO0\_25) will blink at a rate of
4Hz, while the green LED (PIO0\_3) will blink at a rate of 8Hz. The blue LED
(PIO1\_1) is not used.

NOTE: This code only transmits. In the SPI0 configuration you'll find
`SPI_TXCTL_RXIGNORE` is set. If you wish to use DMA to both transmit and
receive at the same time, you'll need to setup a DMA receive transfer.
Basically, anywhere you find a reference to `DMAREQ_SPI0_TX`, you'll need
corresponding code for `DMAREQ_SPI0_RX`.

I found that if you don't set the `SPI_TXCTL_RXIGNORE` option, the DMA handler
would never fire. If you send a single character with
`Chip_SPI_WriteFrames_Blocking()` then the DMA handler would fire. I didn't
delve any deeper in to it, but I suspect the SPI receive overflow error bit
gets set and causes something to happen.  If you figure that out, post it a PR
or an issue and I'll integrate it.

## Build environment

Uses arm-gcc and make. The LPC15xx v2.20 libraries are included. There are two
warnings emitted when `ring_buffer.c` is compiled. I added GCC attributes to
suppress unused variable warnings in the library (who writes code that's
allowed to generate warnings? No excuse!)

