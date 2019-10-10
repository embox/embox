It's an example of LEDs blinking over HTTP.

The HTML page structure should be the following:

| Embox Panel | Leds | Network |
   ____   ____   ____
  |    | |    | |    |
  |LED0| |LED1| |LED2|  ...
  |____| |____| |____|

  ------
  |Save|
  ------

Pressing on LEDs toggles its states. Also you can change Network settings

Pressing "Save" button will save current LEDs state to internal flash memory,
if it's allowed in your mods.config. After rebooting the configuration
will be restored automatically.
