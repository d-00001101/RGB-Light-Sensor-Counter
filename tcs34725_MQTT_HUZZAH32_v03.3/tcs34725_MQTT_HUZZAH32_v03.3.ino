/*
 * RGB sensor detects blue over green color. Gives visual and audible 
 * feedback. Counts the number of times blue is the dominant color
 * and transmits count to an MQTT broker. If count falls below
 * a threshold value in a specified given amount of time, an alert
 * message is transmitted. Includes battery backup power supply.
*/

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "ESP32Tone.h"

#if defined(ARDUINO_ARCH_ESP32)
  bool definedIO = true;
#else
  bool definedIO = false;
#endif


#define VBATPIN A13


const char* ssid       = "PC Guest 2g";
const char* password   = "Venteck99";


/* Mask count vars */
//300000 milliseconds to increment maskCount equal to maskBundleSize
int maskBundleSize = 5;//value may be updated via MQTT callback
unsigned long alarmTimer = 300000;//milliseconds value may be updated via MQTT callback

bool idleAlarmSent = false;
unsigned long maskCount = 0;
bool mask = false;
unsigned long startTime;
unsigned long currentTime;
unsigned long elapsedTime;
float masksPerMinute = 0;
float voltage;
float minimumVoltage = 4650.00;//value may be updated via MQTT callback
bool voltageAlertSent = false;

int LED = 15;
int BUZZER = 27;




void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  char alarmTopic[] = "esp32A\\counter\\alarmTimer";
  char maskBundleSizeTopic[] = "esp32A\\counter\\maskBundleSize";
  char minimumVoltageTopic[] = "esp32A\\counter\\minimumVoltage";

  if (strcmp(topic, maskBundleSizeTopic)==0) {
    maskBundleSize = messageTemp.toInt();
    Serial.print(" New maskBundleSize:  ");
    Serial.print(maskBundleSize);
    Serial.println();
  } else if (strcmp(topic, alarmTopic)==0) {
    alarmTimer = messageTemp.toInt();
    Serial.print(" New alarmTimer:  ");
    Serial.print(alarmTimer);
    Serial.println();
  } else if (strcmp(topic, minimumVoltageTopic)==0) {
    minimumVoltage = messageTemp.toFloat();
    Serial.print(" New minimumVoltage:  ");
    Serial.print(minimumVoltage);
    Serial.println();
  }
}

const char* mqtt_server = "54ebebf010734eeb818a74b993b7caa3.s1.eu.hivemq.cloud";
WiFiClientSecure esp32Client;
PubSubClient mqttClient(mqtt_server, 8883, mqttCallback, esp32Client);




/* Initialise with default values (int time = 2.4ms, gain = 1x) */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_180MS, TCS34725_GAIN_1X);






void setup(void) {
  Serial.begin(9600);
  delay(200);

  Serial.println("IN SETUP");

  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

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
  esp32Client.setInsecure();//skip verification
}








void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (mqttClient.connect("esp32counter01", "esp32counter01", "pa55w0rdY0")) {
      Serial.println("connected");
      mqttClient.subscribe("esp32A\\counter\\maskBundleSize");
      mqttClient.subscribe("esp32A\\counter\\alarmTimer");
      mqttClient.subscribe("esp32A\\counter\\minimumVoltage");

      String messageString = "device,name=\"esp32A\",type=\"counter\" bootup=1";
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








void sendIdleAlarm(unsigned long currentTime) {
    Serial.print("ALARM: "); Serial.print(" "); Serial.println(currentTime - startTime, DEC);
    Serial.print("alarmTimer: "); Serial.print(" "); Serial.println(alarmTimer);
    Serial.print("maskBundleSize: "); Serial.print(" "); Serial.println(maskBundleSize);
    String alarmMessage = "device,name=\"esp32A\",type=\"counter\" idleAlarm=true";
    char alarm[100];
    alarmMessage.toCharArray(alarm, 100);
    mqttClient.publish("sensors/counter", alarm);
}



void sendVoltageAlert(float currentVoltage) {
    String alertMessage = "device,name=\"esp32A\",type=\"counter\" voltage=";
    alertMessage = alertMessage + currentVoltage;
    alertMessage = alertMessage + ",voltageAlert=true";
    char alert[120];

    alertMessage.toCharArray(alert, 120);

    Serial.print("Alert Message: "); Serial.print(" "); Serial.println(alert);
    mqttClient.publish("sensors/counter", alert);
}


void loop(void) {
  
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  //check if esp32 is running on battery power
  voltage = analogRead(VBATPIN);
  voltage = voltage * 2.0;
  if(voltage < minimumVoltage && voltageAlertSent==false) {
    sendVoltageAlert(voltage);
    voltageAlertSent = true;
  } else if (voltage > minimumVoltage) {
    voltageAlertSent = false;
  }

  
  

  //check if counter has been idle
  unsigned long currentTime = millis();
  if (currentTime - startTime > alarmTimer && idleAlarmSent == false) {
    sendIdleAlarm(currentTime);
    idleAlarmSent = true;
  }
  

  //collect RGB data
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);

  b+=150;
  
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  //lux = tcs.calculateLux(r, g, b);

//  Serial.print("I: "); Serial.print(" "); Serial.print(currentIndex*100, DEC); Serial.print(" ");
//  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
//  Serial.print("Lux: "); Serial.print(" "); Serial.print(lux, DEC); Serial.print(" ");
  Serial.print(" R: "); Serial.print(" "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("Count: "); Serial.print(" "); Serial.print(maskCount*100); Serial.print(" ");
  Serial.print(" G: "); Serial.print(" "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print(" B: "); Serial.print(" "); Serial.print(b, DEC); Serial.println(" ");



  //check if RGB values have transitoned from green to blue dominant
  if (b > g && mask == false) {
    Serial.println("-----------B>G-------------");
    Serial.print("alarmTimer: "); Serial.print(" "); Serial.println(alarmTimer);
    Serial.print("maskBundleSize: "); Serial.print(" "); Serial.println(maskBundleSize);
    
    maskCount++;
    mask = true;
    digitalWrite(LED, HIGH);
    tone(BUZZER, 440);
    
    if(maskCount >= maskBundleSize) {
      currentTime = millis();
      elapsedTime = currentTime - startTime;
      startTime = currentTime;

      
      Serial.print(" Voltage: "); Serial.print(" "); Serial.println(voltage, DEC);
      
      Serial.print("ElapsedTime: "); Serial.print(" "); Serial.println(elapsedTime/1000);
      Serial.print("Count: "); Serial.print(" "); Serial.println(maskCount);
      masksPerMinute = (float)maskCount * (60.00/ (elapsedTime/1000));
      Serial.print("Masks per minute: "); Serial.print(" "); Serial.println(masksPerMinute);
      
      String messageElements = "device,name=\"esp32A\",type=\"counter\" count=";
      messageElements = messageElements + maskCount;
      messageElements = messageElements + ",maskspm=";
      messageElements = messageElements + masksPerMinute;
      messageElements = messageElements + ",voltage=";
      messageElements = messageElements + voltage;

      char message[120];

      messageElements.toCharArray(message, 120);

      Serial.print("Message: "); Serial.print(" "); Serial.println(message);
      
      mqttClient.publish("sensors/counter", message);
    
      maskCount = 0;
      idleAlarmSent = false; //reset alarm
    }
  } else if (g > b) {                                 
    mask = false;
    digitalWrite(LED, LOW);
    noTone(BUZZER);
  }
}
