#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
 
#define PIN D2
#define NBPIX 3

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 10 // Set BRIGHTNESS to about 1/5 (max = 255)

char* ssid = "XXX";
char* password = "XXX";

const char* ttn_server = "eu1.cloud.thethings.network";
const char* ttn_user = "draginotl@ttn";
const char *topic = "v3/draginotl@ttn/devices/landrag1/up";
const char* ttn_password = "NNSXS.AV7H454CCTTBWVT774TUTFMCC4OH2KM2SKBI7EQ.x";
const int ttn_port = 1883;
int alarm_status = 0;
WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel leds = Adafruit_NeoPixel(NBPIX, PIN, NEO_GRB + NEO_KHZ800);

void led_set(uint8 R, uint8 G, uint8 B) {
  for (int i = 0; i < NBPIX; i++) {
    leds.setPixelColor(i, leds.Color(R, G, B));
    leds.show();
    delay(500);
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String message;
    for (int i = 0; i < length; i++) {
          message = message + (char) payload[i];  // convert *byte to string
      }
    Serial.print(message);
    Serial.println();
    Serial.println("-----------------------");
    alarm_status = message.indexOf("\"ALARM_status\":false");
    Serial.println(alarm_status);
    if (alarm_status == -1)  { 
      Serial.print("Alarm On");
      led_set(255,0,0);   
        }
    else {
        Serial.print("Alarm Off");        
        led_set(0, 255, 0);      
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Setup MQTT connection");
    // Attempt to connect
    if (client.connect("ESP32Client", ttn_user, ttn_password )) {
      client.setBufferSize(2048);
      Serial.println("MQTT connected");
      client.subscribe(topic);
      Serial.println("MQTT subscribed, waiting for message...");
    } else {
      Serial.print("MQTT failed, rc= ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  leds.begin(); // This initializes the NeoPixel library.
  leds.show();
  leds.setBrightness(BRIGHTNESS);
  led_set(10, 150, 200);    
  WiFi.begin(ssid, password);

  Serial.print("Setup WiFi network ");
  while (WiFi.status() != WL_CONNECTED) {
    led_set(150, 150, 200); 
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  led_set(10, 255, 10);  
  Serial.println("Setup MQTT");
  client.setServer(ttn_server, ttn_port);
  client.setCallback(callback);
  led_set(12, 25, 150);  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
