LPC15xx Simple systick example

Example description:
--------------------
The Systick example simply blinks an LED at a periodic rate using the systick
timer and SYSTICK clock divider. The LED state is toggled in the systick
interrupt handler.
NOTE
While one would expect a clock divider to delay an interrupt occurrence this implementation
seems to hasten the interrupt occurrence. For example, if the SYSTICK clock divider is set
to 1, 453 interrupts occur in a 30-second interval.  If the SYSTICK clock divider is set 
to 2, one would expect to see half the interrupt count however twice the interrupts occure as
though the clock divider were really a clock multiplier.
NOTE
In addition, once the SYSTICK clock divider is set and SYSTICK is enabled, the SYSTICK clock 
divider becomes static, changes seem to have no effect.  Once the clock was running, the effective 
rate could not be changed or the SYSTICK timer clock disabled as described in the UM.


Special connection requirements:
--------------------------------
There are no special connection requirements for this example.

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

