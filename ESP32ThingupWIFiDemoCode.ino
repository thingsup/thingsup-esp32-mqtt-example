/*
  Basic ESP32 MQTT example with Thingsup

  It connects to an Thingusup MQTT broker then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function.

  The code is tested with ESP32 nodeMCU board
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "";                                   // WIFI SSID of network
const char* password = "";                               // WIFI password of the newtowk

const char* mqtt_server = "mqtt.thingsup.io";            // Thingsup Broker URL
const char* mqtt_password = "";                          // Device Password set in the device addition stage in thingsup
const char* mqtt_username = "";                          // Device Key set in the device addition stage in thingsup
String clientId = "";                                    // Client ID set in the device addition stage in thingsup
unsigned int mqtt_port = 1883;                           // MQTT port for SSL connection in thingsup
String accountID = "";

long current_millis = 0;

WiFiClientSecure espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

/*
 * This function sets ESP in STA mode and connects to the WIFI
 * Args: Null
 * Returns: Null
 */

void setup_wifi() {

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
 * This function is callback function which gets triggered with data is recived on the MQTT subscribed topic
 * Args: Null
 * Returns: Topic name, Payload, Length
 */

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/*
 * This function Reconnects to MQTT when there is disconnection
 * Args: Null
 * Returns: Null
 */

void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    Serial.print("clientId : ");
    Serial.println(clientId);

    Serial.print("mqtt_user=");
    Serial.println(mqtt_username);
    Serial.print("mqtt_pass=");
    Serial.println(mqtt_password);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
    {

      Serial.println("connected");
      // Once connected, publish an announcement...
      String publish_topic = "/"  + accountID + "/" + "outTopic";   // The topic name should be always start with /accountID
      client.publish((char*)publish_topic.c_str(), "hello world");
      // ... and resubscribe
      String sub_topic = "/" + accountID + "/" + "inTopic";
      client.subscribe(sub_topic.c_str());
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setClient(espClient);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (millis() - current_millis > 5000)
  {
    current_millis = millis();

    if (client.connected())
    {
      Serial.print("Publish message: ");
      String publish_topic = "/"  + accountID + "/" + "outTopic";   // The topic name should be always start with /accountID
      if (client.publish((char*)publish_topic.c_str(), "hello world") == true)
      {
        Serial.print("Publish sucess: ");
      }
    }
  }
}
