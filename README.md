# Conway's Game Of Life for the Apple II
(an exercise in constrained 8-bit 6502 C programming)

dependencies:
 * https://github.com/cc65/ for compilation
 * http://applecommander.sourceforge.net/ to update disk images.

# Instructions
This repo contains a A2CONWAY.DSK file which can be booted in emulators or transferred to a 140k 5.25" floppy disk.

Upon booting this dsk, a BASIC program called STARTUP will automatically run "A2CONWAY".

A2CONWAY will present some hotkeys and ask that you press enter to start.  After pressing enter, Life will begin with a glider scrolling across the screen.

# Hotkeys
Hotkeys can be pressed at any time during A2CONWAY execution:
 * r: randomize the screen with 400 live cells
 * g: clear the screen and spawn a gosper glider gun
 * s: clear the screen and spawn a simkins glider gun
 * p: pause
 * 1: switch to wraparound naive engine (startup mode)
 * 2: switch to nowrap naive engine
 * 3: switch to wraparound optimized engine
 * 4: switch to nowrap optimized engine
 * q: quit

Note that multiple "engines" are implmented.  The naive engines are very slow, but their code is easier to read.
