LPC15xx Analog Comparator example

Example description:
--------------------
The ACMP example demonstrates using the analog comparator.
On the NXP LPCXpresso 1549 board, this example configures the positive voltage 
input as the voltage found on ADC1_1/PIN0.9, which is trimmed through the 
potentiometer.
On the KEIL MCB1500 board, this example configures the positive voltage input
as the voltage found on the ACMP_I1/PIN0.27.  To supply a variable voltage a
wire jumper can be connected between the potentiometer (POT_MOT1 pin 3) and 
PIO0_27/ACMP_I1 (MOTOR2-pin 17).
The negative voltage input is obtained from the voltage ladder, the
voltage ladder output is set to around 50% of Vdd.
Adjust the POT up and down to adjust the voltage into the analog
comparator. When the voltage crosses the negative voltage input,
a CMP IRQ is fired. Based on which side of the voltage is in
reference to the bandgap, the LED state will change.

Special connection requirements:
--------------------------------
On the KEIL MCB1500 board connect a wire jumper between the potentiometer 
(POT_MOT1 pin 3) and PIO0_27/ACMP_I1 (MOTOR2-pin 17).

Build procedures:
-----------------
Visit the  LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

