# Embedded ESP32-S2 microcontroller Smart Curtain System using ESP-IDF framework written in C/C++.

The purpose of this project was to give me more insights and experience in an embedded development environment and solve a real world issue in my home. I have three giant 6 foot wide windows in my apartment which allows a lot of heat in and reduces the privacy in my house. I wanted to build a device that can stow all curtains from external environment events such as a timer, heat sensor, or voice command. Since each window is seperated with a supportive beam in between, my original plan was to make 3 seperate esp devices that would interlink and syncronize together. However, if I were to do this project again, I would replace the 3 devices with a single devices and a motor that is capable enough of rotating a 20ft PVC pipe with a 7lbs load. In order to communicate with each device independently, each ESP32 will poll a private Django webserver with HTTP requests once per second. For more information on the Django webserver, see this repository here: https://github.com/RoscoeTheDog/RoscoeTheDogWebHub

# Hardware requirements:
* ESP32-S2-Saola microcontroller
* Push button actuators-- I used some arcade style arrow ones to fit the up-down elevator design of the curtains.
* TMC2209 stepper driver controller (microstepping for silence and extra precision)
* A stepper motor. I specifically used a Nema17 but I would use a Nema19 or a worm gear motor if I were to redesign and do the project again.
* Power Source-- I used a 12v 8000mah battery to match the stepper motor.
* 12v to 5v buck converter or equivillent to stepdown power source for controller
* Solar panels (optional)
* Solar charge controller (optional)

# Known bugs:
* Wifi settings are not restored from NVS storage upon cold boot. You must sign into wifi using EspTouch Smartconfig Android/iOS app to reconnect.
* The magnetic switches which detect when curtains are fully stowed are causing the TaskWatchdog monitor thread service to panic and reset when 'actuated'

# Known hardware issues:
* The current CAD design for the smart blinds works and is compact but the nema17 motors inside are just barely strong enough to rotate a 6 foot PVC pipe at reasonable speeds. I would like to put a wormgear motor inside instead with enough torque to rotate a ~20ft pipe holding about 5-7 lbs.


Getting Started with this project:

Download VSCode
Download VSCode "platformio" extension

All platformIO projects have a platformio.ini configuration file
double check your monitor speed and upload ports are configured correctly and operating on a valid COM port
monitor_speed = 115200
upload_port = COM10

if you are using an ESP-prog breakout development board for debugging, then be sure to run it at a very low speed as it's unstable at higher speeds

```
debug_tool = esp-prog
debug_speed = 500
```

To specify which pins on the microcontroller you will be using for what hardware (motors, sensors, etc), you will need to navigate to the `configGpio.h` file located in the `include` project directory.

Each hardware peripherial used on the board will have a seperate `.c` configuration source file located in the `src` folder. The file `globals.c` is where to set the values for for these peripherials. This includes the specifications for whatever Nema17 or equivillent stepper motor is used, the rod diameter for the curtains, the length of them, motor speed, etc.

A webserver is needed to read and save the requests from a smartphone to the smart blinds device. In my testing, I was using IFTT, as it is a free service that integrates with Google Voice very easily. An IFTT command would send an HTTP request with body of JSON data to the webserver, which it would then parse and save in it's database. When the smart blinds polls and requests the servers data every one second, it will update the global variables states in the firmware, and then move to the motors to the corresponding positions.