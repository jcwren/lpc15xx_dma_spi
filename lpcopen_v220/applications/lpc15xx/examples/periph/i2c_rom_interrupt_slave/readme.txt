LPC15xx I2C bus slave (interrupt) example using the ROM API

Example description:
--------------------
This example shows how to configure I2C as a bus slave in interrupt mode using 
the ROM-based APIs.
This demo supports both 7-bit and 10-bit addressing, but only 7-bit addressing is
used in the example. After I2C is setup, the I2C receive and transmit functions
are called through the built-in ROM routines.
If this demo is running correctly and is correctly connected to a master that
supports this example, the LED will toggle. Because of the connection
requirements, this example is best used as a reference on how to use the ROM
API I2C slave functions.

Special connection requirements:
--------------------------------
IMOPRTANT NOTE:
This example requires a connection via I2C to a I2C master. The I2C master writes a byte
to the slave which controls the slave's LED state (0 or 1). The master can also read the
slave's LED state (0 or 1).

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

