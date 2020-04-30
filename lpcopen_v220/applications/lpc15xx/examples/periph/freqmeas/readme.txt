LPC15xx Freqeuency measurement example

Example description:
--------------------
This example shows how to use the frequency measurement capability in
the SYSCON and INMUX blocks to measure unknown frequencies using a known
reference frequency.
The frequency measurement capability can measure target source frequencies
if the reference frequency is greater than the target frequency. The example
will measure the RTC and watchdog oscillator frequencies using the main
oscillator, internal RC oscillator, and the divided system clock (via the
CLKOUT pin) as reference clocks.

Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

