#include <Arduino.h>
#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial //for USB communication 
#define SerialAT Serial1 //for communication with the LTE module
#define SerialTen Serial2 //need this if we are going the teensey route
#define TINY_GSM_DEBUG SerialMon
// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
// These defines are only for this example; they are not needed in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
/*
TODO: 
1. write the code needed to send CAN frames from teensy to the esp 32 vie the serial 2 ports. This will need some sort of ID
bits or like an ID byte inorder to specify what kind of byte array is coming from the teensy.
 -figure out how to efficiently add some ID bytes/bits to the start of very CAN frame that gets sent to the serial monitor
 this would allow for ease of implementation and classification later on.
2. implement a fucntion that take information from the serial 2 pins and sort them into thier respective 
topics for sending to cloud.
 -Remove the ID byte or bits from the data incoming, this will most likely be done with some header nad external file to make the code less
 fucking ugly 
3. Work on getting the topics to properly send into the correct channels.
 -Convert to sending byte arrays rather than the goofy shit I hva currently 
 -Specify all the topics based on the CAN on google sheets. 
*/

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

const char* mqtt_server = "4.tcp.ngrok.io"; //replace with ngrok from mpache
const char* mqtt_username = "gr24"; // replace with your Username
const char* mqtt_password = "gr24"; // replace with your Password
const int mqtt_port = 11018;


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

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient  mqtt(client);



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

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we’re reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection… ");
    String clientId = "ESP32Client";
    // Attempt to connect
    if (mqtt.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected!");
      // Once connected, publish an announcement…
      mqtt.publish("testTopic", "connected!");
      // … and resubscribe
      mqtt.subscribe("testTopic");
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
    Serial.begin(115200);
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

    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS)) {
        Serial.println("SDCard MOUNT FAIL");
    } else {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        String str = "SDCard Size: " + String(cardSize) + "MB";
        Serial.println(str);
    }

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

#if TINY_GSM_USE_WIFI
    // Wifi connection parameters must be set before waiting for the network
    SerialMon.print(F("Setting SSID/password..."));
    if (!modem.networkConnect(wifiSSID, wifiPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
    // The XBee must run the gprsConnect function BEFORE waiting for network!
    modem.gprsConnect(apn, gprsUser, gprsPass);
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

    // MQTT mqtt_server setup
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqttCallback);

}
unsigned long lastMsg;

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
{

    // Make sure we're still registered on the network
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

    if (!client.connected()) {
    reconnect();
  }
  //this is used to check if mqtt pipeline is working, can be deleted later on
  byte abc[16] = {0xb4,0xaf,0x98, 0xb4,0xaf,0x98, 0xb4,0xaf,0x98, 0xb4,0xaf,0x98,0xb4,0xaf,0x98, 0x98};
  mqtt.loop();
  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    char msg[16];
    itoa(millis(), msg, 10);
    Serial.print("Publish message: ");
    Serial.println(msg);
    mqtt.publish("meta", abc, 16);
  }
  /*
  char chtemp;
  Serial.print(Serial.available() > 0);
  if(Serial.available() > 0){
    chtemp = Serial.read();
    Serial.print(chtemp);
  }
  */
}