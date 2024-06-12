#include <Arduino.h>
#include "send_nodes.h"
#include "ssl_client.h"
#include <WiFi.h>
//#include "ca_cert.h"

#define SerialMon Serial //for USB communication 
#define SerialAT Serial1 //for communication with the LTE module

//needed for Serial to be decoded
bool request;
get_nodes result;
unsigned long requestTime;
bool clear;
unsigned long lastMsg;

const char *ssid = "BK1031 iPhone"; 
const char *password = "pogchamp"; 

// MQTT details

const char* mqtt_server = "mapache.gauchoracing.com"; //replace with ngrok from mpache
const char* mqtt_username = "gr24"; // replace with your Username
const char* mqtt_password = "gr24"; // replace with your Password
const int mqtt_port = 1883;


#include <PubSubClient.h>


WiFiClient esp32;
PubSubClient mqtt(esp32);


int ledStatus = LOW;
uint32_t lastReconnectAttempt = 0;

//mqtt node names for loop stuff
String mqtt_nodes[] = {
 "gr24/inverter",
 "gr24/vdm",
 "gr24/wheel1",
 "gr24/wheel2",
 "gr24/wheel3",
 "gr24/wheel4",
 "gr24/IMU",
 "gr24/GPS",
 "gr24/Pedals",
 "gr24/ACU",
 "gr24/BCM",
 "gr24/Dash",
 "gr24/EM"
};


void send(){
  unsigned long now = millis();
  result.inverter[43] = (byte) now;
  result.inverter[42] = (byte) (now >> 8);
  result.inverter[41] = (byte) (now >> 16);
  result.inverter[40] = (byte) (now >> 24);
  
  result.vdm[51] = (byte) now;
  result.vdm[50] = (byte) (now >> 8);
  result.vdm[49] = (byte) (now >> 16);
  result.vdm[48] = (byte) (now >> 24);
  
  result.wheel1[43] = (byte) now;
  result.wheel1[42] = (byte) (now >> 8);
  result.wheel1[41] = (byte) (now >> 16);
  result.wheel1[40] = (byte) (now >> 24);
  
  result.wheel2[43] = (byte) now;
  result.wheel2[42] = (byte) (now >> 8);
  result.wheel2[41] = (byte) (now >> 16);
  result.wheel2[40] = (byte) (now >> 24);
  
  result.wheel3[43] = (byte) now;
  result.wheel3[42] = (byte) (now >> 8);
  result.wheel3[41] = (byte) (now >> 16);
  result.wheel3[40] = (byte) (now >> 24);
  
  result.wheel4[43] = (byte) now;
  result.wheel4[42] = (byte) (now >> 8);
  result.wheel4[41] = (byte) (now >> 16);
  result.wheel4[40] = (byte) (now >> 24);
  
  result.IMU[27] = (byte) now;
  result.IMU[26] = (byte) (now >> 8);
  result.IMU[25] = (byte) (now >> 16);
  result.IMU[24] = (byte) (now >> 24);
  
  result.GPS[35] = (byte) now;
  result.GPS[34] = (byte) (now >> 8);
  result.GPS[33] = (byte) (now >> 16);
  result.GPS[32] = (byte) (now >> 24);
  
  result.Pedals[19] = (byte) now;
  result.Pedals[18] = (byte) (now >> 8);
  result.Pedals[17] = (byte) (now >> 16);
  result.Pedals[16] = (byte) (now >> 24);
  
  result.ACU[403] = (byte) now;
  result.ACU[402] = (byte) (now >> 8);
  result.ACU[401] = (byte) (now >> 16);
  result.ACU[400] = (byte) (now >> 24);
  
  result.TCM[11] = (byte) now;
  result.TCM[10] = (byte) (now >> 8);
  result.TCM[9] = (byte) (now >> 16);
  result.TCM[8] = (byte) (now >> 24);
  
  result.Dash[27] = (byte) now;
  result.Dash[26] = (byte) (now >> 8);
  result.Dash[25] = (byte) (now >> 16);
  result.Dash[24] = (byte) (now >> 24);
  
  result.EM[19] = (byte) now;
  result.EM[18] = (byte) (now >> 8);
  result.EM[17] = (byte) (now >> 16);
  result.EM[16] = (byte) (now >> 24);
  
  mqtt.publish("gr24/test/inverter", result.inverter, 44);
  mqtt.publish("gr24/test/vdm", result.vdm, 52);
  mqtt.publish("gr24/test/bcm/fr", result.wheel1, 44);
  mqtt.publish("gr24/test/bcm/fl", result.wheel2, 44);
  mqtt.publish("gr24/test/bcm/rr", result.wheel3, 44);
  mqtt.publish("gr24/test/bcm/rl", result.wheel4, 44);
  mqtt.publish("gr24/test/bcm/imu", result.IMU, 28);
  mqtt.publish("gr24/test/imu/gps", result.GPS, 36);
  mqtt.publish("gr24/test/pedal", result.Pedals, 20);
  mqtt.publish("gr24/test/acu", result.ACU, 404);
  mqtt.publish("gr24/test/tcm", result.TCM, 12);
  mqtt.publish("gr24/test/dash", result.Dash, 28);
  mqtt.publish("gr24/test/em", result.EM, 20);


}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  mqtt.publish("gr24/test/pong", payload, 8);
}

void reconnect() {
  while (!mqtt.connected()) {
    String clientId = "TCM";
    if (mqtt.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected!");
      mqtt.subscribe("gr24/test/ping");
    } else {
      Serial.print("failed, rc = ");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup()
{
    Serial.setRxBufferSize(1024);
    Serial.begin(115200);
    request = false;
    clear = true;
    delay(10);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqttCallback);
    mqtt.setKeepAlive(5);
    mqtt.setBufferSize(1024);

}


void loop()
{     
    if (!esp32.connected()) {
        Serial.println(WiFi.status());
        reconnect();
    }
    while (WiFi.status() != WL_CONNECTED) {
    Serial.print("NC_WIFI");
    delay(1000);
    }
    //code from es32 serial
    //sends a request every time its ready to recieve more data
    byte data[768];
    if(!request){
        Serial.write(0x06);
        //Serial.println("here");
        requestTime = millis();
        request = true;
    }

    //every second try to read serial and if nothing clears the buffer
    unsigned long now_packet = millis();
    if(now_packet - requestTime > 1000){
        requestTime = now_packet;
        Serial.print(Serial.available());
        Serial.print("clear");
        if(clear){
        while(Serial.available()){
            Serial.read();
        }
        }
        Serial.write(0x06);
        clear = false;
    }

    //serial read
    if(Serial.available() >= 768){
        Serial.readBytes(data, 768);
        clear = true;
        request = false;
        //Serial.print("read");
        result.take_nodes(data);
        unsigned long now_mqtt = millis();
        if (now_mqtt - lastMsg > 50) {
        lastMsg = now_mqtt;
        send();
    }
    }

    //no clue what mqtt.loop() does but its probably important
    //it is infact important LMAO
    mqtt.loop();   
}

