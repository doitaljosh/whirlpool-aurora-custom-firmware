# Whirlpool Aurora LED board custom firmware

## Here, you'll find Arduino-based firmware I developed for the LED controller inside some Whirlpool fridges.

The Aurora LED board is used to drive the interior LED lighting of some Whirlpool fridges, and normally talks to the fridge's
main board over a proprietary protocol called WIDE. It includes two constant current LED drivers, and two 12V PWM output channels
to control a 12V heater and shelf lighting. Driving all this is an STM8S105K4T6 MCU. Fortunately, SDCC and Sduino both support this
chip, so I have wrote an open source firmware to leverage the features of this board without reverse engineering Whirlpool's protocol.
My firmware supports a simple custom serial protocol that tells the board what LED's to turn on and what brightness levels to set
them at. I've included demo code that tells the board to fade both LED channels from off to on slowly. The board uses a single-wire 
half-duplex serial interface, so a tri-state buffer is required.

## Aurora board pinouts:

### P1:
1. +12V
2. NC
3. RX/TX
4. GND

## P3:
1. LED1+
2. LED1-
3. LED2+
4. LED2-

## P6:
(Connect to STLINK)
1. +5V
2. RST
3. GND
4. SWIM

# Serial protocol:

## Frame format:
### [0xDD][u8 ledBoardAddr][u8 Cmd][u8 Channel][u8 Value][PWM1][PWM2][u8 Checksum]

## Fields:
### Addr: 
(Multiple boards can be controlled on one bus. Unique for each board)
1. Master addr: 0x00
2. LED board addr: 0x01

### Command:
1. 0x00: Control (LED ch1 and DC load ch1) or (LED ch2 and DC load ch2) or all
2. 0x01: Control LED ch1 or LED ch2 or both
3. 0x02: Control DC load ch1 or DC load ch2 or both

### Channel:
1. 0x00: Channel 1
2. 0x01: Channel 2
3. 0x02: Both channels

### Value:
1. 0x00: Disable
2. 0x01: Enable

### Brightness:
0x00-0xFF

## Command response:
### Frame format: [0xDD][masterAddr][Cmd][Checksum]


There is a lot more to be added. Pre-set modes, control of the PWM controlled 12V outputs, and a more efficient protocol layer are to name a few.
