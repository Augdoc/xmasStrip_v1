xmasStrip_v1
============
Interactive light display using LPD8806 LED strips

This code was developed to take advantage of the programmable LPD8806 strips.  It has
been updated to work with the Arduino IDE version 1.0 and higher, and the adafruit
LPD8806 library.

The strips can be obtained from adafruit.  They also have nice tutorials on the setup.
Make sure you use a dedicated power supply with proper amperage for the strip.
	http://www.adafruit.com/products/306
	
Videos of a 5M strip in action can be found here:
	http://www.youtube.com/watch?v=09an1RehNVQ
	http://www.youtube.com/watch?v=Z1m0e04-AI0


HARDWARE SETUP

An arduino Uno will handle a 5M strip (160 LEDs) easily.  I used a Mega 2560 so that
I could have a dedicated hardware serial port for the MIDI.  Because I have a second
array to store pixel brightness, you will run out of RAM quickly on an Uno if you
go much beyond 160 LEDs.

A "mode" button is wired to pin 3 (interrupt 1).  The four drum piezos are wired to
analog inputs 0-3.  Each analog input is also connected (via diodes) to pin 2
(interrupt 0).  While it makes the wiring a bit more complex, I found it easier to
have an interrupt handle the drum logic, rather than constantly polling the analog
pins.


SOFTWARE SETUP

The softare operates in two modes: light (display) and drum (interactive) mode.  Each
mode has several programs containing different light patterns.  These are just a few
I came up with.  (The rainbow patterns are borrowed from ada.)  Hopefully others can
contribute more.  I am not the best commenter in the world, so use at your own risk.
