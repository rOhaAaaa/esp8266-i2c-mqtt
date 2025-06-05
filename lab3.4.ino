#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <time.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "307_2.4G";
const char* password = "29012006";

const char* mqtt_server = "fb4fcf99cc2d416695328fd37cd76c33.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "ESP8266V";
const char* mqtt_pass = "Esp8266V";
const char* mqtt_topic = "emqx/esp8266";

const char* weather_host = "api.openweathermap.org";
const char* weather_apiKey = "60ccb24be1c161727d4b4053fef98a71";
const char* city = "Lviv";
const char* country = "UA";

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

float temp = 0.0;
String weather = "Unknown";
String lastMqttMessage = "No MQTT msg";
unsigned long lastWeatherUpdate = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  lastMqttMessage = "";
  for (unsigned int i = 0; i < length; i++) {
    lastMqttMessage += (char)payload[i];
  }
  Serial.print("MQTT message: ");
  Serial.println(lastMqttMessage);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connectToMQTT() {
  secureClient.setInsecure();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("MQTT connected");
      mqttClient.subscribe(mqtt_topic);
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}

void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 100000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTime synced");
}

void fetchWeather() {
  if (secureClient.connect(weather_host, 443)) {
    String url = "/data/2.5/weather?q=" + String(city) + "," + country +
                 "&units=metric&appid=" + weather_apiKey;

    secureClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                       "Host: " + weather_host + "\r\n" +
                       "Connection: close\r\n\r\n");

    while (secureClient.connected()) {
      String line = secureClient.readStringUntil('\n');
      if (line == "\r") break;
    }

    String payload = secureClient.readString();
secureClient.stop();

int jsonIndex = payload.indexOf('{');
if (jsonIndex == -1) {
  Serial.println("JSON start not found!");
  return;
}
payload = payload.substring(jsonIndex);

Serial.println("Raw JSON:");
Serial.println(payload);

DynamicJsonDocument doc(4096);
DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print("Weather JSON parse error: ");
  Serial.println(error.f_str());
} else {
  temp = doc["main"]["temp"] | 0.0;
  weather = doc["weather"][0]["main"].as<String>();
}

  } else {
    Serial.println("Weather connect failed");
  }
}

void showInfo() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);

  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", t->tm_hour, t->tm_min);

  char dateStr[12];
  sprintf(dateStr, "%02d.%02d.%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== Weather ===");
  display.print("City: ");
  display.println(city);
  display.print("Temp: ");
  display.print(temp, 1);
  display.println(" C");
  display.print("Cond: ");
  display.println(weather);
  display.println("------------------");
  display.print("Time: ");
  display.println(timeStr);
  display.print("MQTT: ");
  display.println(lastMqttMessage.substring(0, 15));
  display.display();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting up...");
  display.display();

  connectToWiFi();
  setupTime();
  connectToMQTT();
  fetchWeather();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  if (millis() - lastWeatherUpdate > 60000) {
    fetchWeather();
    lastWeatherUpdate = millis();
  }

  showInfo();
  delay(1000);
}
