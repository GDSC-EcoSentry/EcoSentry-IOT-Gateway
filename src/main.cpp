// include the library
#include <Arduino.h>
#include <RadioLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// Replace with your network credentials
const char* ssid = "Okaeri";
const char* password = "cocktail";

// nRF24 has the following connections:
// CS pin:    D8
// IRQ pin:   D1
// CE pin:    D0
nRF24 radio = new Module(D8, D1, D0);

// URL
//String URL = "https://us-central1-gdsc-ecosentry.cloudfunctions.net/app/update?stationID=1&nodeID=1&co=100&dust=200&humidity=300&soil_moisture=400&temperature=500&rain=200";

typedef struct sensor_struct {
  float temp;
  float humid;
  int rain;
  int moisture;
} sensor_struct;

void setFlag(void);
int sendRequest(String URL);
void radioError(int state);
sensor_struct messageParsing(String input);
String buildURL(sensor_struct data);

void initRadio() {
  // initialize nRF24 with default settings
  Serial.print(F("[nRF24] Initializing ... "));
  int state = radio.begin();
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }

  // set receive pipe 0 address
  // NOTE: address width in bytes MUST be equal to the
  //       width set in begin() or setAddressWidth()
  //       methods (5 by default)
  Serial.print(F("[nRF24] Setting address for receive pipe 0 ... "));
  byte addr[] = {0x01, 0x23, 0x45, 0x67, 0x89};
  state = radio.setReceivePipe(0, addr);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }

  // set the function that will be called
  // when new packet is received
  radio.setPacketReceivedAction(setFlag);

  // start listening
  Serial.print(F("[nRF24] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void initWifi() {
  //Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}

void setup() {
  Serial.begin(9600);

  initRadio();
  initWifi();
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we got a packet, set the flag
  receivedFlag = true;
}

void loop() {
  // check if the flag is set
  if(receivedFlag) {
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    sensor_struct sensor_data = messageParsing(str);
    
    String URL = buildURL(sensor_data);
    Serial.println(URL);

    radioError(state);

    // put module back to listen mode
    radio.startReceive();

    sendRequest(URL);
  }
}

// Parse received messages
sensor_struct messageParsing(String input) {
  // Parsing stuff
  int commaIndex = input.indexOf(',');
  int secondCommaIndex = input.indexOf(',', commaIndex + 1);
  int thirdCommaIndex = input.indexOf(',', secondCommaIndex + 1);

  sensor_struct result;
  result.temp = input.substring(0, commaIndex).toFloat();
  result.humid = input.substring(commaIndex + 1, secondCommaIndex).toFloat();
  result.rain = input.substring(secondCommaIndex + 1, thirdCommaIndex).toInt();
  result.moisture = input.substring(thirdCommaIndex + 1).toInt(); // To the end of the string
  return result;
}

String buildURL(sensor_struct data) {
  String string_test = "https://us-central1-gdsc-ecosentry.cloudfunctions.net/app/update?stationID=1&nodeID=1&humidity=";
  // * 100 to avoid float in URL
  string_test += (int) (data.humid * 100);
  string_test += "&soil_moisture=";
  string_test += data.moisture;
  string_test += "&rain=";
  string_test += data.rain;
  string_test += "&temperature=";
  string_test += (int) (data.temp * 100);
  return string_test;
}

//TODO: Take care of return code
int sendRequest(String URL) {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, URL)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  return 0;
}

void radioError(int state) {
  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 32 bytes
    Serial.println(F("too long!"));

  } else if (state == RADIOLIB_ERR_ACK_NOT_RECEIVED) {
    // acknowledge from destination module
    // was not received within 15 retries
    Serial.println(F("ACK not received!"));

  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    // timed out while transmitting
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }
}