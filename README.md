# SSE-HUD-Arduino-Code
Different scripts that are run on the HUD Arduino.
Please email john (john@umbriac.com) if you have any questions, I would be happy to help!

## prototype_CAN_HUD_board
This is the code that runs on the HUD arduino during normal operation. It will collect CAN data and output it to the USB port or the bluetooth module.

It also has working code for calculating wheelspeed directly from a hall effect sensor input, and infinished code for reading the lap button

## How to flash a bluetooth module

 1. Flash the arudino with the code in the flash_bluetooth_settings folder.
 2. Power down the arduino.
 3. Hold the button on the HC-05 bluetooth module while restoring power to the arduino.
 4. Keep holding the button on the HC-05 bluetooth module for 10 seconds
 5. Power the arudino down again.

 The bluetooth module should now be flashed with the correct settings. 
