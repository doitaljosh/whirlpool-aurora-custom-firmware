### Whirlpool Aurora LED board custom firmware

# Here, you'll find Arduino-based firmware I developed for the LED controller inside some Whirlpool fridges.

The Aurora LED board is used to drive the interior LED lighting of some Whirlpool fridges, and normally talks to the fridge's
main board over a proprietary protocol called WIDE. It includes two constant current LED drivers, and two 12V PWM output channels
to control a 12V heater and shelf lighting. Driving all this is an STM8S105K4T6 MCU. Fortunately, SDCC and Sduino both support this
chip, so I have wrote an open source firmware to leverage the features of this board without reverse engineering Whirlpool's protocol.
My firmware supports a simple custom serial protocol that tells the board what LED's to turn on and what brightness levels to set
them at. I've included demo code that tells the board to fade both LED channels from off to on slowly. The board uses a single-wire 
half-duplex serial interface, so a tri-state buffer is required.

## Aurora board pinouts:

# P1:
1. +12V
2. NC
3. RX/TX
4. GND

# P3:
1. LED1+
2. LED1-
3. LED2+
4. LED2-