LPC15xx Temperature sensor example

Example description:
--------------------
The LPC15xx temperature sensor example shows how to monitor the
chip temperature using the build-in temperature sensor and ADC.
The ADC is configured to monitor the temperature sensor input and
is sampled by manually starting the ADC sequence from the system
tick interrupt. The sequence then starts and generates an interrupt
when complete.
Note: The recommended approach to read the temperature sensor
requires using an ADC burst and reading a later sample instead of the
initial sample. This examples uses this approach, but displays both
the later sample and initial sample values for comparison.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

