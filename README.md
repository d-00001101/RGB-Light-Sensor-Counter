# RGB-Light-Sensor-Counter
A real world light color sensor used with an external MQTT broker &amp; InfluxDB to remotely track, measure and alert manufacturing progress.

# Table of Contents
1. [Project Description](#project-description)  
2. [IOT Network](#iot-network-configuration)  
3. [Required Tools](#required-tools) 
4. [Hardware Components](#hardware-components) 
5. [Modules and Interfaces](#modules-and-interfaces)  
6. [Wiring](#wiring)  
7. [The Code](#the-code)  
8. [Conclusion](#conclusion)  

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

<img src="https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/RGB%20Light%20Sensor%20Counter.png" width=50% height=50%>

The completed item counter consists of 3 modules with 3 interfaces. The first and most important module is the TSC34725 housed within a 3d printed piece that exposes the sensor and LED outward in a light reflector similar to a flashlight. This module connects to a custom printed breakout board. The interface between the sensor and the breakout board is a 5 pin JST connector with jumper wires at the opposite end. The custom PCB interfaces with the Adafruit HUZZAH32 via headers. Finally, the HUZZAH32 is powered by a USB cable.

<img src="https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/Light%20Funnel%20part2.jpg" width=60% height=60%>

# Wiring

<img src="https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/tsc34725%20Pinout.jpg" width=50% height=50% >

The TSC34725 has 7 pinouts.

* **LED**: ActiveLow LED
* **INT**: Interrupt
* **SDA**: I2C Data
* **SLC**: I2C Clock
* **3V3**: 3.3 Volt Power Supply
* **GND**: Ground
* **VIN**: 5 Volt Power Supply

The **LED** pinout is ActiveLow so we will only create a connection to future proof the project should there ever be a need to control the LED. As it stands, the LED will turn on by default. It will be connected to GPIO #32 on the HUZZAH32. The **INT** pinout will not be used in this project so we will ignore it for now. The **SDA** and **SCL** pinouts will be connected to their corresponding I2C GPIO pins on the HUZZAH32. The HUZZAH32 has a 3V pin that can supply 500mA peak which should be more than enough to power the TSC34725. We will connect the **3V** pin to the **3V3** pin on the TSC34725 and ignore its 5 volt VIN pin. Lastly, the **GND** pin will be connected to HUZZAH32's corresponding ground pin.

![Alt](https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/Connections.png)

The diagram shows a buzzer and LED connected to the HUZZAH32. The connections are implemented in the custom PCB but we will not use them in this project. They are merely there to future proof this project should there ever be a need to create an audible or visual signal that an item has been counted.

![Alt](https://github.com/d-00001101/RGB-Light-Sensor-Counter/blob/main/assets/imgs/breakout.jpg)

# The Code
We start by including all the external libraries that will be needed. **WiFiClientSecure** provides Client SSL to the ESP32. **PubSubClient** provides a client for doing simple publish/subscribe messaging to an MQTT server. **Wire** is a library that allows us to communicate with I2C devices. **Adafruit_TCS34725** is a driver for the Adafruit TCS34725 Breakout module.

We then make sure the board is using an ESP32 chip as opposed to an ESP8266 as they are dependent on different libraries (I use this mainly for debugging purposes because I don't use ESP32s exclusively). Then we declare the battery pin to measure the battery's voltage. If the voltage falls below an adjustable threshold value, it will send a voltage alert which may suggest possible tampering. This is followed by declaring default values which can be updated via the air by publishing a message to their MQTT topic. This is followed by declaring the global state variables. The state variables are responsible for triggering side effects. We will do this by memoizing them and checking for the inflection moment.

```
  #include <WiFiClientSecure.h>
  #include <PubSubClient.h>
  #include <Wire.h>
  #include "Adafruit_TCS34725.h"
  
  #if defined(ARDUINO_ARCH_ESP32)
    bool definedIO = true;
  #else
    bool definedIO = false;
  #endif
  
  #define VBATPIN A13
  
  const char* ssid       = "YOUR_WIFI_NETWORK";
  const char* password   = "YOUR_WIFI_PASSWORD";
  
  /* Declare default values */
  //300000 milliseconds to increment maskCount equal to maskBundleSize
  int maskBundleSize = 5;//value may be updated via MQTT callback
  unsigned long alarmTimer = 300000;//milliseconds value may be updated via MQTT callback
  float minimumVoltage = 4650.00;//value may be updated via MQTT callback
  
  /* Declare state variables */
  bool idleAlarmSent = false;
  unsigned long maskCount = 0;
  bool maskIsPresent = false;
  unsigned long startTime;
  unsigned long currentTime;
  unsigned long elapsedTime;
  float masksPerMinute = 0;
  float voltage;
  bool voltageAlertSent = false;
```

The **mqttCallBack** function responds to events like when a message is received. This function compares the message topic to the topics this mqttClient has subscribed to, and if there is a match, it does something useful with the payload. In this case, we register the callback with the mttClient in order to remotely publish updated values to the sensor (from anywhere in the world!) and therefore override the default global variables. Then we initialize the TSC34725 sensor using a very fast integration time so that we may get high frequency sensor readings.

```
//Function will run upon message arrival
void mqttCallback(char* topic, byte* message, unsigned int length) {
 Serial.print("Message arrived on topic: ");
 Serial.print(topic);
 Serial.print(". Message: ");
 String messageTemp;
 
 for (int i = 0; i < length; i++) {
   Serial.print((char)message[i]);
   messageTemp += (char)message[i];
 }

 char alarmTopic[] = "esp32_01\counter\alarmTimer";
 char maskBundleSizeTopic[] = "esp32_01\counter\maskBundleSize";
 char minimumVoltageTopic[] = "esp32_01\counter\minimumVoltage";

 if (strcmp(topic, maskBundleSizeTopic)==0) {
   maskBundleSize = messageTemp.toInt();
   Serial.print(" New maskBundleSize:  ");
   Serial.print(maskBundleSize);
   Serial.println();
 } 
 
 if (strcmp(topic, alarmTopic)==0) {
   alarmTimer = messageTemp.toInt();
   Serial.print(" New alarmTimer:  ");
   Serial.print(alarmTimer);
   Serial.println();
 }
 
 if (strcmp(topic, minimumVoltageTopic)==0) {
   minimumVoltage = messageTemp.toFloat();
   Serial.print(" New minimumVoltage:  ");
   Serial.print(minimumVoltage);
   Serial.println();
 }
}

const char* mqtt_server = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX.s1.eu.hivemq.cloud";
WiFiClientSecure esp32Client;
PubSubClient mqttClient(mqtt_server, 8883, mqttCallback, esp32Client);
/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_180MS, TCS34725_GAIN_1X);
```

The **setup** function starts the sensor, connects the HUZZAH32 to the local WiFi network, and establishes the **startTime** global value.

```
void setup(void) {
 Serial.begin(9600);
 delay(200);

 Serial.println("IN SETUP");

 if(definedIO) {
   Serial.print("ESP32 DEFINED");
 } else {
   Serial.print("ESP32 NOT DEFINED");
 }
 
 if (tcs.begin()) {
   Serial.println("Found sensor");
 } else {
   Serial.println("No TCS34725 found ... check your connections");
   while (1);
 }

 setup_wifi();

 //setup timer
 startTime = millis();
}
```

The **setup_wifi** function determines how the HUZZAH32 logs in to the local network. In this example I omited the TLS (Transport Layer Security) support for two reasons. It is beyond the scope of this blog post, there are plenty of examples online to make the cumbersome process simple, and every third party MQTT broker will have instructions on how to set up TLS in their MQTT server.

```
void setup_wifi() {
 delay(100);
 // connect to a WiFi network
 Serial.println();
 Serial.printf("Connecting to %s", ssid);

 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }

 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());

 //client.setCACert(CACertificate_here);
 //I'm ommiting TLS support in this example.
 //Any implementation of TLS support will depend in great part on your MQTT broker settings.
 //skip verification - not advised
 esp32Client.setInsecure();
}
```

The **reconnect** function runs in the main loop of our program and tries to connect to the MQTT broker every 5 seconds until it is successful. When and if it is successful, it begins to listen for messages by subscribing to 3 topics that refer to this sensor's identifying topic - its name: "esp32_01", followed by the category of sensor: "counter", and the data description: "maskBundleSize", "alarmTimer", and "minimumVoltage". Then it publishes a message informing the database that it has booted up.

```
void reconnect() {
 // Loop until we're reconnected
 while (!mqttClient.connected()) {
   Serial.print("Attempting MQTT connection...");
     // Attempt to connect
     if (mqttClient.connect("esp32counter01", "esp32counter01", "ESP32COUNTER01_PASSWORD")) {
     Serial.println("connected");
     mqttClient.subscribe("esp32_01\counter\maskBundleSize");
     mqttClient.subscribe("esp32_01\counter\alarmTimer");
     mqttClient.subscribe("esp32_01\counter\minimumVoltage");

     String messageString = "device,name=\"esp32_01\",type=\"counter\" bootup=1";
     char message[120];
     messageString.toCharArray(message, 120);
     mqttClient.publish("sensors/counter", message);
   } else {
     Serial.print("failed, rc=");
     Serial.print(mqttClient.state());
     Serial.println(" try again in 5 seconds");
     // Wait 5 seconds before retrying
     delay(5000);
   }
 }
}
```

These 2 functions send alarm messages to the database

```
void sendIdleAlert(unsigned long currentTime) {
   Serial.print("ALARM: "); Serial.print(" "); Serial.println(currentTime - startTime, DEC);
   Serial.print("alarmTimer: "); Serial.print(" "); Serial.println(alarmTimer);
   Serial.print("maskBundleSize: "); Serial.print(" "); Serial.println(maskBundleSize);
   String alarmMessage = "device,name=\"esp32_01\",type=\"counter\" idleAlarm=true";
   char alarm[100];
   alarmMessage.toCharArray(alarm, 100);
   mqttClient.publish("sensors/counter", alarm);
}



void sendVoltageAlert(float currentVoltage) {
   String alertMessage = "device,name=\"esp32_01\",type=\"counter\" voltage=";
   alertMessage = alertMessage + currentVoltage;
   alertMessage = alertMessage + ",voltageAlert=true";
   char alert[120];

   alertMessage.toCharArray(alert, 120);

   Serial.print("Alert Message: "); Serial.print(" "); Serial.println(alert);
   mqttClient.publish("sensors/counter", alert);
}
```

The **loop** function is the main function of the program. This is where the magic happens. First we establish connection to the MQTT broker and call the client's loop function to listen and receive messages.

```
void loop(void) {
 
 if (!mqttClient.connected()) {
   reconnect();
 }
 mqttClient.loop();
 
 . . . 
```

We then determine if a voltage alert must be sent. We read the battery's pin to get the current voltage reading. HUZZAH32's voltage pin sits behind a divider so we multiply the voltage by 2 in order to get the accurate value. If a voltage alert has not been sent and the voltage is less than the minimumVoltage, we send the alert and change the voltageAlertSent state to true so that only one alert is sent. If current voltage is more than minimumVoltage, voltageAlertSent state remains false or we reset it to false.

```
. . . 

 //check if esp32 is running on battery power
 voltage = analogRead(VBATPIN);
 voltage = voltage * 2.0;
 if(voltageAlertSent==false && voltage < minimumVoltage) {
   sendVoltageAlert(voltage);
   voltageAlertSent = true;
 } else if (voltage > minimumVoltage) {
   voltageAlertSent = false;
 }
 
 . . .
```

Next we determine if an idle alert must be sent. If the elapsed time is more than the alarmTimer and the idleAlarmSent is false, we send one alarm and change the idleAlarmSent state to true.

```
. . . 

 //check if counter has been idle
 unsigned long currentTime = millis();
 if (idleAlarmSent == false && currentTime - startTime > alarmTimer) {
   sendIdleAlert(currentTime);
   idleAlarmSent = true;
 }
 
 . . .
```

Lastly we handle the TSC34725 readings. First we capture the the sensor's RGB values. If blue light is greater than green and maskIsPresent has not yet been set to true, we first increment the total maskCount and set the maskIsPresent state to true. the we determine if the maskCount is equal to or more than the desired maskBundleSize. If it is, we calculate the elapsed time since the previous maskBundleSize was exceeded. We will need this data to calculate the masksPerMinute value. We also reset the startTime to begin the timer anew.

```
. . .

 //collect RGB data
 uint16_t r, g, b, c;
 tcs.getRawData(&r, &g, &b, &c);
 
 //Serial.print(" R: "); Serial.print(" "); Serial.print(r, DEC); Serial.print(" ");
 //Serial.print("Count: "); Serial.print(" "); Serial.print(maskCount*100); Serial.print(" ");
 //Serial.print(" G: "); Serial.print(" "); Serial.print(g, DEC); Serial.print(" ");
 //Serial.print(" B: "); Serial.print(" "); Serial.print(b, DEC); Serial.println(" ");



 //check if RGB values have transitoned from green to blue dominant
 if (b > g && maskIsPresent == false) {
   //Serial.println("-----------B>G-------------");
   //Serial.print("alarmTimer: "); Serial.print(" "); Serial.println(alarmTimer);
   //Serial.print("maskBundleSize: "); Serial.print(" "); Serial.println(maskBundleSize);
   
   maskCount++;
   maskIsPresent = true;
   
   if(maskCount >= maskBundleSize) {
     currentTime = millis();
     elapsedTime = currentTime - startTime;
     startTime = currentTime;

     
     //Serial.print(" Voltage: "); Serial.print(" "); Serial.println(voltage, DEC);
     
     //Serial.print("ElapsedTime: "); Serial.print(" "); Serial.println(elapsedTime/1000);
     //Serial.print("Count: "); Serial.print(" "); Serial.println(maskCount);
     masksPerMinute = (float)maskCount * (60.00/ (elapsedTime/1000));
     //Serial.print("Masks per minute: "); Serial.print(" "); Serial.println(masksPerMinute);

. . .
```

Finally we concatenate the MQTT message elements into a string adhering to InfluxDB's line protocol which is a text based format for writing points to the database. The elements consist of a measurement labeled "device", followed by two tags: name and type. The tags are followed by comma separated fields that include the mask count, masks per minute, and the current battery voltage. Then we publish the message to a topic that the company server is subscribed to. The server then relays the message to InfluxDB.

```diff
-Update, a company server is no longer needed. InfluxDB has an MQTT client 
-that can subscribe to the topic directly.
```

We then reset the maskCount to zero, the idleAlarmSent state to false and the mask bundle counting begins again.

If the green light is greater than the blue, it means there is no mask present and so none of the above happens and all we do is set the maskIsPresent state to false.

```
. . .

     String messageElements = "device,name=\"esp32_01\",type=\"counter\" count=";
     messageElements = messageElements + maskCount;
     messageElements = messageElements + ",maskspm=";
     messageElements = messageElements + masksPerMinute;
     messageElements = messageElements + ",voltage=";
     messageElements = messageElements + voltage;

     char message[120];

     messageElements.toCharArray(message, 120);

     //Serial.print("Message: "); Serial.print(" "); Serial.println(message);
     
     mqttClient.publish("sensors/counter", message);
   
     maskCount = 0;
     idleAlarmSent = false; //reset alarm
   }
 } else if (g > b) {                                 
   maskIsPresent = false;
 }
}
```

# Conclusion
The IOT revolution has empowered us to truly work smarter with other added benefits. The purpose of the sensors was to reduce loss, measure production, and enable us to better prioritize our time. It succeeded in that endeavor. What was surprising is the unintended effect the sensors had on employee behavior that security cameras didn't. If we are defined by what we do when no one is watching and security cameras have blind spots, sensors transcend the effects of appearance by establishing a basis count by which we can all abide.
