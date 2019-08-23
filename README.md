# STM32
In this Rep you will find the codes I wrote for the 32bit micro-controleur STM32. 
These were made to detect the rising edge of the GPS signal and to calculate its period. 
To use these codes, you will need the libraries libopencm3. 
Also, I worked on a STM32F407 so if you are using another one, you might have to change some things in the code. 

The files input_capture.c and input_capture.h contain the fonctions to set the input capture, etc. 

If you just want to blink a led every time you have a rising edge, you just have to use the blink_ic.c. With the main.c, you can
blink a LED and measure the period of the signal. You'll read the period with the serial port. 
