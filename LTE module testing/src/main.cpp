#include <Arduino.h>
#include "send_nodes.h"
#include "ssl_client.h"
#include <WiFi.h>
//#include "ca_cert.h"

#define TINY_GSM_MODEM_SIM7600

#define SerialMon Serial //for USB communication 
#define SerialAT Serial1 //for communication with the LTE module

#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI false

//needed for Serial to be decoded
bool request;
get_nodes result;
unsigned long requestTime;
bool clear;
unsigned long lastMsg;

const char *ssid = "BK1031 iPhone"; 
const char *password = "pogchamp"; 

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
// const char apn[]      = "YourAPN";
const char apn[]      = "mobile.three.com.hk";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "xx";
const char wifiPass[] = "xx";

// MQTT details

const char* mqtt_server = "mapache.gauchoracing.com"; //replace with ngrok from mpache
const char* mqtt_username = "gr24"; // replace with your Username
const char* mqtt_password = "gr24"; // replace with your Password
const int mqtt_port = 1883;

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <SPI.h>
#include <SD.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif
//define DUMP_AT_COMMANDS
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif
TinyGsmClient client(modem);

//WiFiClient esp32;
//PubSubClient  mqtt(esp32);
// TinyGsm modem(SerialAT);
// TinyGsmClient client(modem);
// SSLClient ssl_client(&tcpClient);
PubSubClient mqtt(client);

Ticker tick;



#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  60          // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD   115200
//#define MODEM_DTR     25
#define MODEM_TX      27
#define MODEM_RX      26
#define MODEM_PWRKEY  4
//#define MODEM_DTR     32
#define MODEM_RI      33
#define MODEM_FLIGHT  25
#define MODEM_STATUS  34

#define SD_MISO     2
#define SD_MOSI     15
#define SD_SCLK     14
#define SD_CS       13
#define LED_PIN     12

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
  mqtt.publish("gr24/test/vdm", result.inverter, 52);
  mqtt.publish("gr24/test/bcm/fr", result.wheel1, 44);
  mqtt.publish("gr24/test/bcm/fl", result.wheel2, 44);
  mqtt.publish("gr24/test/bcm/rr", result.wheel3, 44);
  mqtt.publish("gr24/test/bcm/rl", result.wheel4, 44);
  mqtt.publish("gr24/test/bcm/imu", result.IMU, 28);
  mqtt.publish("gr24/test/imu/gps", result.inverter, 36);
  mqtt.publish("gr24/test/pedal", result.Pedals, 20);
  mqtt.publish("gr24/test/acu", result.ACU, 404);
  mqtt.publish("gr24/test/tcm", result.TCM, 12);
  mqtt.publish("gr24/test/dash", result.Dash, 28);
  mqtt.publish("gr24/test/em", result.EM, 20);


}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  mqtt.publish("gr24/test/pong", payload, 8);
  // for (int i = 0; i < length; i++) {
    
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();
}

void reconnect() {
  // Loop until we’re reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection… ");
    String clientId = "TCM";
    // Attempt to connect
    if (mqtt.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected!");
      // Once connected, publish an announcement…
      //mqtt.publish("testTopic", "connected!");
      // … and resubscribe
      mqtt.subscribe("gr24/test/ping");
    } else {
      Serial.print("failed, rc = ");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
    // Set console baud rate
    Serial.setRxBufferSize(1024);
    Serial.begin(115200);
    request = false;
    clear = true;
    delay(10);
    SerialAT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
    
    // Set LED OFF
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    pinMode(MODEM_PWRKEY, OUTPUT);
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(300);
    digitalWrite(MODEM_PWRKEY, LOW);


    /*
    MODEM_FLIGHT IO:25 Modulator flight mode control,
    need to enable modulator, this pin must be set to high
    */
    pinMode(MODEM_FLIGHT, OUTPUT);
    digitalWrite(MODEM_FLIGHT, HIGH);

    // SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    // if (!SD.begin(SD_CS)) {
    //     Serial.println("SDCard MOUNT FAIL");
    // } else {
    //     uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    //     String str = "SDCard Size: " + String(cardSize) + "MB";
    //     Serial.println(str);
    // }

    Serial.println("\nWait...");

    delay(1000);


    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DBG("Initializing modem...");
    if (!modem.init()) {
        DBG("Failed to restart modem, delaying 10s and retrying");

    }

    String ret;
    //    do {
    //        ret = modem.setNetworkMode(2);
    //        delay(500);
    //    } while (ret != "OK");
    ret = modem.setNetworkMode(2);
    DBG("setNetworkMode:", ret);

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) {
        modem.simUnlock(GSM_PIN);
    }
#endif

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected()) {
        SerialMon.println("Network connected");
    }

#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) {
        SerialMon.println("GPRS connected");
    }
#endif
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi ..");
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print('.');
  //   delay(1000);
  // }
  // Serial.println(WiFi.localIP());

  //   // MQTT mqtt_server setup
  //   mqtt.setServer(mqtt_server, mqtt_port);
  //   mqtt.setCallback(mqttCallback);

}


//not really needed
void gen_random(const int len, char *res) {
    static const char alphanum[] =
        "0123456789"
        "abcdef";
    for (int i = 0, p = 0; i < len; ++i) {
        res[i] = alphanum[int(random(0, sizeof(alphanum) - 1))];
    }
  char tmp[16];
  itoa(millis(), tmp, 10);
  strcat(res, tmp);
}


void loop()
{     //connections statements for network and gps
    if (!modem.isNetworkConnected()) {
        SerialMon.println("Network disconnected");
        if (!modem.waitForNetwork(180000L, true)) {
            SerialMon.println(" fail");
            delay(10000);
            return;
        }
        if (modem.isNetworkConnected()) {
            SerialMon.println("Network re-connected");
        }

    #if TINY_GSM_USE_GPRS
        // and make sure GPRS/EPS is still connected
        if (!modem.isGprsConnected()) {
            SerialMon.println("GPRS disconnected!");
            SerialMon.print(F("Connecting to "));
            SerialMon.print(apn);
            if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
                SerialMon.println(" fail");
                delay(10000);
                return;
            }
            if (modem.isGprsConnected()) {
                SerialMon.println("GPRS reconnected");
            }
        }
    #endif
    }

    //tiny gsm force reconnect
    
    if (!client.connected()) {
        reconnect();
    }
    
    // if (!esp32.connected()) {
    //     reconnect();
    // }
    // while (WiFi.status() != WL_CONNECTED) {
    // Serial.print("NC_WIFI");
    // delay(1000);
    // }
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
        //Serial.print(Serial.available());
        //Serial.print("clear");
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
    mqtt.loop();   
}

