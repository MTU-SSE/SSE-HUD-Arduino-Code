# SSE-HUD-Arduino-Code
Different scripts that are run on the HUD Arduino.
Please email john (john@umbriac.com) if you have any questions, I would be happy to help!

## prototype_CAN_HUD_board
This is the code that runs on the HUD arduino during normal operation. It will collect CAN data and output it to the USB port or the bluetooth module.

It also has working code for calculating wheelspeed directly from a hall effect sensor input, and infinished code for reading the lap button

## flash_bluetooth_settings
This is what it sounds like. This will flash a connected HC-05 bluetooth module with our desired settings. To use this script, upload it to the arduino, then power the arduino down. Hold the button on the HC-05 module while restoring power to the arduino. Wait ~10 seconds, and power the arudino down again. The bluetooth module should now be flashed with the correct settings. 
