# Blinking LED Memory
A simple memory game!

## Description
Game starts at level 1. The LEDs will light up in a random order, press the buttons in the same order to advance to
next level. The sequence will grow for each level.

## Instructuions
When the arduino boots up it will be in idle, this can be seen by all the
LEDs are being ON. Press the button 1 (One of the corner buttons) to start.

If the sequence which the buttons are entered correct, same as the LED pattern, a 'winning' sequence is displayed.
on the LEDs and after a couple of seconds the next round will start. The same sequence are displayed when starting all levels. 
If wrong sequence is entered, you will have one last try. If you would fail on this attemt as well, the LEDs will do a game-over sequence 
and go back to idle. There is total 5 levels.

Would you like to end, or reset the game, hold down the button 1 for a minimum of 1,5 seconds. After this the game will go back to idle.

Good Luck!

## Board 
Current default board is now UNO. Hardcoded HW PIN values in the code. 

Wiring can bee seen in the **doc** directory.
