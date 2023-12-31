
# Raspberry Pi Pico MKU UP 2424 Serial Interface Control

  

This project utilizes the Raspberry Pi Pico to control the MKU UP 2424 transceiver through its serial interface and automatically switch it between TX and RX based on the state of the GPIOs

  

## Installing:

1. Download the binary ([9600 Baud](https://raw.githubusercontent.com/luki34567780/tx_controller/master/firmware_9600.uf2), [115200 Baud](https://raw.githubusercontent.com/luki34567780/tx_controller/master/firmware_115200.uf2))

2. Push down the 'boot' button on your Raspberry pi pico

3. Plug your pico into your computer (while keeping the 'boot' button pressed)

4. Release the 'boot' button. A USB drive called 'RPI-RP2' should appear

5. Copy the UF2 file you downloaded in step 1 onto the drive. The pico should now automatically reboot

## Features

- Very fast switching (<2ms at 9600 Baud, <0.25ms at 115200 Baud)
- TX LED
  

## Known Issues and Pitfalls:

- If the transceiver is not reacting, try swapping RX and TX. A RS232 cable normally should have TX and RX crossed, but this is sometimes not the case.
- If the transceiver is not reacting, check that you have the binary with the correct BAUD (the old firmware has a baud of 9600, the new one has 115200)
- If you're using a pico W the builtin LED won't work. 


If you're still having issues you can open a issue.


# License

MIT License

Copyright (c) 2023 Lukas Seitz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.