<h1>Automatic watering of plants with M5 core2</h1>
This is a small project to water plants using M5 core2. It is implemented in C++ only and uses only VS code and it's plugin PlatformIO.<br>
<img src="https://github.com/Freshman25/M5Core_Water_Plants/blob/main/WateringOverview.jpg?raw=true" alt="Finished Project" width="400" height="430">


## Overview
- [Parts and wiring](#Parts-and-wiring)
- [PlatformIO and VS Code](#Platformio-and-vs-code)
- [Application](#Application)
## Parts and wiring


I am using the M5 Core2. According to the pin chart on the back of the microcontroller, Port A offers pins, which deliver PA_SDA and PA_SCL. I've got reading issues from this Port A, so i instead decided to re-wire to an ADC port. As the sensor of the pump did not perform very well, i use the unit earth additionally. If you have experience with microcontrollers and values of the pump are ok, you can use these instead and skip this chapter.<br>
Parts:
<ul>
  <li>M5 Core2 (Other ones also ok)</li>
  <li>Unit Watering (SKU:U101)</li>
  <li>Unit Earth (SKU:U019)</li>
  <li>3 Jump wires</li>
</ul>
<p>
Connect Unit Watering: Connect Unit Watering to Port A. To turn the pump ON/OFF PIN 32 is used on M5 Core2.<br>
Connect Unit Earth: I use jump wires to connect Unit Earth to the pin board on the back. I connect one jump wire to a ground pin (GND written on the back/it's black). The voltage of unit earth is 5V (written on back of unit), thus i connect it to a 5V slot. Lastly, i connect to an ADC PIN, i choose G35 (G36 is also available). Please check voltage prior to connecting to prevent damage of parts.
</p>
Finished, I have the following setup:
<img src="https://github.com/Freshman25/M5Core_Water_Plants/blob/main/Connections.jpg?raw=true" alt="Connections" width="400" height="450">

<p>Remark: While the unit watering (SKU:U101), comes with a moisture sensor, I am using the unit earth in addition. If you are using a different version of m5 microcontrollers you might consider doing it on one port, but I will not go into depth for that.</p>

## PlatformIO and VS Code
I am using VS Code and install PlatformIO as plugin. Once the plugin has finished installing, i can activate it by clicking on the PlatformIO symbol on the left-hand side and choose to create a new project. I choose the applicable microcontroller (M5 core2) and wait until the setup completes. The first setup takes about 10min on my slow machine. Then, I change the library to M5Core2.h (#include <M5Core2.h>) and add the lib into the platformio.ini . This is to configure the buttons and touch correctly, but if you don't need it/only want to use the screen the M5.Unified library can also be used. To verify the setup, I connect to the M5 Core2 module by cable and use the PlatformIO:Upload symbol in the lower left-hand corner to build the standard program. As this worked without errors, I am ready to go.
## Application
Prior to development of the web interface, i test the humidity reader and pump functionality. Therefore, I provided the file tester.cpp . Copy it to you main.cpp and let it build to test. Make sure you have configured the correct pins.<br>
<p>
Once the functionality is confirmed, I develop the webinterface and finish the small application. The microcontroller needs to be in the same network as my development computer to see it.
  <img src="https://github.com/Freshman25/M5Core_Water_Plants/blob/main/PlantBarometer3.png?raw=true" alt="WebInterface" width="270" height="450"><br>
  The image is hosted by Wikipedia, thus would like to state the author's credits: Donovan Govan., CC BY-SA 3.0 http://creativecommons.org/licenses/by-sa/3.0/ via Wikimedia Commons
</p>
Lastly I would like to mention that the microcontroller does not see the file system of the computer. In order to use e.g. extermal html/local images they need to be transferred to the microcontroller first/bridged differently. I did not do that. I just used available graphics and transferred strings for my web application that are read as html in the end.

