#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include "webpage.h"

#define BLUE_PIN    18
#define YELLOW_PIN  19
#define BUZZER_PIN  32
#define RELAY_PIN   33
#define SMOKE_PIN   34
#define TEMP_PIN    27
#define BUTTON_PIN  25

const int SMOKE_THRESHOLD = 1900;
const float TEMP_THRESHOLD = 40.0;

const char* ssid = "Roha";
const char* password = "39865475";

WiFiServer server(80);

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

float currentTemperature = 0;
int currentSmokeLevel = 0;

bool alarmActive = false;
bool overThresholdNow = false;
bool wasOverThreshold = false;
unsigned long alarmSuppressUntil = 0;

#define MAX_EVENTS 100
String eventLog[MAX_EVENTS];
int eventCount = 0;

const char* ntpServer = "129.6.15.28"; 
const long gmtOffset_sec = 3 * 3600;
const int daylightOffset_sec = 0;

Preferences preferences;

void setup() {
  Serial.begin(115200);

  Serial.println("\n🔄 Підключення до Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("✅ Wi-Fi підключено!");
  Serial.print("🔗 IP ESP32: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("🌐 Синхронізація часу через Інтернет");

  struct tm timeinfo;
  int retry = 0;
  const int retry_count = 10;
  while (!getLocalTime(&timeinfo) && retry < retry_count) {
    Serial.println("⌛ Очікуємо синхронізацію часу...");
    delay(500);
    retry++;
  }
  if (retry == retry_count) {
    Serial.println("⚠️ Помилка синхронізації часу!");
  } else {
    Serial.println("✅ Час синхронізовано!");
  }

  server.begin();
  sensors.begin();

  pinMode(BLUE_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SMOKE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(BLUE_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  noTone(BUZZER_PIN);

  loadEventLog(); 
}

void loop() {
  readSensors();
  handleClient();
  handleButton();
}

String getDateTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "00.00 00:00:00";
  }
  char buffer[20];
  sprintf(buffer, "%02d.%02d %02d:%02d:%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return String(buffer);
}

void saveEventLog() {
  preferences.begin("eventLog", false);
  preferences.putInt("count", eventCount);
  for (int i = 0; i < eventCount; i++) {
    String key = "event" + String(i);
    preferences.putString(key.c_str(), eventLog[i]);
  }
  preferences.end();
}

void loadEventLog() {
  preferences.begin("eventLog", true);
  eventCount = preferences.getInt("count", 0);
  if (eventCount > MAX_EVENTS) eventCount = MAX_EVENTS;
  for (int i = 0; i < eventCount; i++) {
    String key = "event" + String(i);
    eventLog[i] = preferences.getString(key.c_str(), "");
  }
  preferences.end();
}

void logEvent(String message) {
  if (eventCount >= MAX_EVENTS) {
    for (int i = MAX_EVENTS - 1; i > 0; i--) {
      eventLog[i] = eventLog[i-1];
    }
    eventCount = MAX_EVENTS - 1;
  }
  for (int i = eventCount; i > 0; i--) {
    eventLog[i] = eventLog[i-1];
  }
  eventLog[0] = getDateTimeString() + " " + message;
  eventCount++;
  saveEventLog();
  sendEventToServer(message); 
}

void sendEventToServer(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String encodedMessage = urlencode(message);
    String serverPath = "http://192.168.1.102/add_event.php?event=" + encodedMessage;
    http.begin(serverPath);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.println("✅ Подію відправлено на сервер!");
    } else {
      Serial.print("❌ Помилка надсилання: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("⚠️ Wi-Fi не підключено!");
  }
}

String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) code0 = c - 10 + 'A';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

void readSensors() {
  currentSmokeLevel = analogRead(SMOKE_PIN);
  sensors.requestTemperatures();
  currentTemperature = sensors.getTempCByIndex(0);

  Serial.print("Smoke: ");
  Serial.print(currentSmokeLevel);
  Serial.print(" | Temp: ");
  Serial.print(currentTemperature);
  Serial.println(" °C");

  overThresholdNow = (currentSmokeLevel > SMOKE_THRESHOLD || currentTemperature > TEMP_THRESHOLD);

  if (overThresholdNow && !wasOverThreshold && millis() > alarmSuppressUntil) {
    Serial.println("🚨 Перевищення!");
    logEvent("🚨 Зафіксовано перевищення (ТРИВОГА)");
    activateAlarm();
  }

  if (!overThresholdNow && wasOverThreshold) {
    Serial.println("✅ Норма відновилась");
    logEvent("✅ Показники нормалізувались");
    deactivateAlarm();
  }

  if (alarmActive && overThresholdNow) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }

  wasOverThreshold = overThresholdNow;
}

void clearEventLog() {
  eventCount = 0;
  preferences.begin("eventLog", false);
  preferences.clear();
  preferences.end();
  Serial.println("🧹 Історію подій очищено!");
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("🖥️ Новий клієнт!");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("GET /reset") >= 0) {
      Serial.println("🔓 Тривога скинута через ВЕБ-кнопку");
      alarmActive = false;
      deactivateAlarm();
      alarmSuppressUntil = millis() + 10000;
    }

    if (request.indexOf("GET /clear") >= 0) {
      Serial.println("🧹 Очищення історії через ВЕБ-кнопку");
      clearEventLog();
    }

    if (request.indexOf("GET /data") >= 0) {
      String json = generateJSON(currentTemperature, currentSmokeLevel, alarmActive, eventLog, eventCount);
      client.print("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
      client.print(json);
    } else {
      String response = generateWebPage(currentTemperature, currentSmokeLevel, alarmActive, eventLog, eventCount);
      client.print(response);
    }

    delay(1);
    client.stop();
    Serial.println("Клієнт відключений.");
  }
}

void handleButton() {
  static unsigned long buttonPressStart = 0;

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (buttonPressStart == 0) {
      buttonPressStart = millis();
    } else if (millis() - buttonPressStart >= 2000 && alarmActive) {
      Serial.println("🔓 Тривогу скинуто фізичною кнопкою");
      alarmActive = false;
      deactivateAlarm();
      alarmSuppressUntil = millis() + 10000;
      buttonPressStart = 0;
    }
  } else {
    buttonPressStart = 0;
  }
}

void activateAlarm() {
  alarmActive = true;
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(RELAY_PIN, HIGH);
}

void deactivateAlarm() {
  alarmActive = false;
  digitalWrite(BLUE_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW); 
}
