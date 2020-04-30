LPC15XX USB Composite device example

Example description:
--------------------
The example shows how to us USBD ROM stack to creates a composite device. A USB
device which has multiple class interface in single configuration is termed as
composite device. In this example and virtual comm port interface (using CDC-ACM class) and
a HID-mouse interfaces are combined together to form a composite device. The VCOM interface
could be used for out putting debug messages.

Special connection requirements:
--------------------------------
Connect the USB cable between micro connector on board and to a host.
When connected to Windows host use the .inf included with the LPCOPen example
Windows USB driver package. You can get this package on the LPCOpen download
page at 
http://www.lpcware.com/content/nxpfile/lpcopen-platform.
For OSx (Mac) host, no drivers are needed.

Build procedures:
-----------------
Visit the LPCOpen quickstart guides to get started building LPCOpen projects.
[Link: http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides]

