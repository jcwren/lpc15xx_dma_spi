LPC15xx Digital to Analog Converter example

Example description:
--------------------
The DAC example demonstrates using the Digital to Analog Converter.
This example uses the DAC to produce a stepped saw tooth waveform at 100Hz.
It uses the internal timer as a trigger source for generating DAC interrupts
The timer is set up to generate interrupts at 2KHz i.e. 20 DAC output samples
for every sawtooth waveform. At the occurrence of the DAC interrupt the DAC input
register is updated with an incremented value and the value is reset after it
crosses the 12bit boundary for DAC input. P0.12 is the set up as the DAC output
By connecting an oscilloscope to P0.12 and ground you will notice a stepped sawtooth 
waveform at 100Hz.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

