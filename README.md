# RGB-Light-Sensor-Counter
A real world light color sensor used with an external MQTT broker &amp; InfluxDB to remotely track, measure and alert manufacturing progress.

# Table of Contents
1. Project Description  
2. IOT Network  
3. Required Tools  
4. Hardware Components  
5. Modules and Interfaces  
6. Wiring  
7. The Code  
8. Conclusion  

# Project Description
The problem this project was meant to solve revolves around the challenge of tracking production output of a product that is in a different continent, is 2 dimensional in shape and is small enough to be easily pilfered in small to medium quantities on the factory floor. These quantities, multiplied by personnel and days easily add up to a quantity that can not be considered negligible. In this instance, the items in question are 3 ply surgical masks, AKA COVID masks.

The goal of this project was to create an item counter that can handle up to 160 items per second, record the number to an external time series database via MQTT protocol, and send alerts when production count falls below an adjustable threshold value. It must also be tamper proof while adhering to a 'modules and interfaces' framework in order to use components that can be easily swapped.

# IOT Network Configuration
In formulating our IOT infrastructure, we established certain parameters. The first parameter was to rely as little as possible on the host country's telecommunications infrastructure. Not all internets are the same and some are downright spotty and slow. This limitation led us to choose MQTT as our transmission protocol due to its light footprint. The second parameter was to rely, as much as possible, on third party SAAS(s) and PAAS(s) rather than hosting our own. It is tempting to run your own server but given the current quality of third party services, it no longer makes sense to allocate time and effort running your own server. Services like Google Cloud, AWS and in our case, HiveMQ and InfluxData makes it incredibly easy to have a dependable and secure back-end. The third parameter was to limit the number of tasks assigned to HUZZAH32 to the bare minimum. We wanted to give the HUZZAH32 one task, capture and transmit the data. I will confess that we failed to meet this parameter. The HUZZAH32 turned out to be such a powerful little board, that it ended up gobbling any task we threw at it and so its code grew appropriately.

We decided that our RGB sensor would send light weight MQTT messages every 5 minutes to a HiveMQ MQTT broker. A company server would subscribe to the broker and transmit the messages to InfluxData which provides a real-time graph dashboard. (Of course, you may use a DB of your choice.)

![Alt](https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/IOT%20Network.gif)

```diff
-Update: Since writing this blog InfluxData has implemented its own MQTT client via the
-new Influx DB Native Collectors feature as part of its service. This new service simplifies
-the configuration by eliminating the need to use company server to forward sensor messages
-to InfluxData. The code provided remains with the exception that the 
-MQTT network no longer needs a company server.
```

# Required Tools
1. Crimping Pliers
2. Soldering Kit
3. Drill
4. Drill Bits - 2mm to 10mm

# Hardware Components
1. TCS34725 RGB Color Sensor
2. Adafruit HUZZAH32 – ESP32 Feather Board
3. Adafruit Lithium Ion Polymer – 3.7v 500mAh Battery
4. JST Connector – XH2.54 5 Pin Terminal Header
5. 5 Female Jumper Wires of Different Colors
6. Electrical Plastic Case 3.94 x 2.68 x 1.97 inch
7. 3D printed Light Funnel / Sensor Housing
8. Breadboard or Custom PCB
9. Male to Male USB Cable (Micro B to USB Type A)

![Alt](https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/Hardware%20Components.png)

The TCS34725 RGB Color Sensor is the perfect light sensor for this task. It has an IR blocking filter integrated on-chip and localized to the color sensing photodiodes. It also minimizes the IR spectral component of the incoming light and allows color measurements to be made accurately with an incredible 3,800,000:1 dynamic range. Given these capabilities, the item counter is able to sense the change in color as the newly manufactured masks move along a conveyor belt.

The Adafruit HUZZAH32 is a beautiful and powerful little board that has built in USB-to-Serial converter, automatic bootloader reset, Lithium Ion/Polymer charger that enables it to have a backup power supply, a dual-core ESP32 chip, 4 MB of SPI Flash, tuned antenna, Wifi/Bluetooth capability, and enough processing power to handle all our project requirements.

JST connectors along with a little soldering will enable us to pre fabricate components that can be easily plugged in to a custom PCB. This will make the item counter compact and the modules easy to repair and change.

# Modules and Interfaces
![Alt](https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/RGB%20Light%20Sensor%20Counter.png)

The completed item counter consists of 3 modules with 3 interfaces. The first and most important module is the TSC34725 housed within a 3d printed piece that exposes the sensor and LED outward in a light reflector similar to a flashlight. This module connects to a custom printed breakout board. The interface between the sensor and the breakout board is a 5 pin JST connector with jumper wires at the opposite end. The custom PCB interfaces with the Adafruit HUZZAH32 via headers. Finally, the HUZZAH32 is powered by a USB cable.

![Alt](https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/Light%20Funnel%20part2.jpg)
