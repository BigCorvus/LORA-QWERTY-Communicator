# LORA-QWERTY-Communicator
A tidy and feature-packed LORA QWERTY communication device mainly based on a Blackberry Q10 keyboard, nRF52840, SX1262 and a 2.7'' Sharp Memory LCD (LS027B7DH01).
I created this because I wanted to practice a little and put some components that I had laying around to good use. The system had to have a low power consumption and outdoor usability, hence the component choices. The SX1262 draws 4,6mA in RX mode and the nRF52 can poll the keyboard while also drawing little power. Without optimizing anything I got 12mA. This can be further reduced by turning off some sensors. The unit can monitor its own power consumption and battery charge status via a BQ27441 Lithium Fuel Gauge.  
![finished devices](https://github.com/BigCorvus/LORA-QWERTY-Communicator/blob/main/Q10%20Lora%20Communicator/Images/20220512_214537.jpg)  

There is a passive piezo buzzer and a vibration motor for notification purposes. There is a footprint for a BME280, which I included as an altimeter. A DS3231M RTC is supposed to keep time. A MPU9250 9-DOF IMU was included to act as a tilt-compensated compass or for air mouse functionality. A GD25Q16CE QSPI flash (works with mass storage TinyUSB test sketch) for storage of for example chat history. An optional GPS module can be connected to the exposed connector at the top of the device. I used a BN220 which fits into the enclosure if you use an appropriate LiPo cell.  
![open devices](https://github.com/BigCorvus/LORA-QWERTY-Communicator/blob/main/Q10%20Lora%20Communicator/Images/20220512_214741.jpg)  

The repository contains the EAGLE PCB design files, gerber files for JLCPCB, Solidworks design files and .STL for a perfectly fitting enclosure and an Arduino test sketch to test most of the system components.
While the hardware is confirmed working (only one minor bug had to be fixed in the prototype - conflicting I2C addresses), the firmware part is very raw and has yet to be developed. Unfortunately I lack time desperately for such things at the moment. What I had in mind for this device is mainly something like an encrypted LORA communicator, maybe eben based on Meshtastic. It can also communicate with other BLE devices and acquire, log and share sensor data or act as a USB or BLE HID keyboard.  

![bottom](https://github.com/BigCorvus/LORA-QWERTY-Communicator/blob/main/Q10%20Lora%20Communicator/Images/q10-lora-bottom.png)  
![top](https://github.com/BigCorvus/LORA-QWERTY-Communicator/blob/main/Q10%20Lora%20Communicator/Images/q10-lora-top.png)  
![schematics](https://github.com/BigCorvus/LORA-QWERTY-Communicator/blob/main/Q10%20Lora%20Communicator/Images/q10-lora-v1.1-schematic.png)



