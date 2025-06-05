# IoT проект: ESP8266 + OLED + MQTT + OpenWeatherMap

## 🔧 Опис
Проєкт з дисципліни "Інтернет речей (IoT)" — відображення:
- Температури та стану погоди через [OpenWeatherMap API](https://openweathermap.org/)
- Повідомлень з брокера [HiveMQ Cloud MQTT](https://www.hivemq.com/mqtt-cloud-broker/)

## 🧠 Функціонал
- ESP8266 з'єднується з Wi-Fi
- Синхронізує час через NTP
- Отримує поточну погоду по місту (наприклад, Lviv)
- Показує дані на OLED-дисплеї (через I²C)
- Підключається до HiveMQ MQTT, підписується на топік, приймає повідомлення

## 🧪 Технології
- ESP8266 (NodeMCU)
- OLED 128x64 (I²C)
- MQTT (HiveMQ Cloud)
- OpenWeatherMap API
- Бібліотеки:
  - `Adafruit_SSD1306`
  - `Adafruit_GFX`
  - `PubSubClient`
  - `ArduinoJson`
  - `WiFiClientSecure`

## ⚙️ Приклад MQTT
Топік: `emqx/esp8266`

При надсиланні повідомлення з MQTT Web Client:


## 📋 Опис
Цей проєкт демонструє використання мікроконтролера **ESP8266** для:
- отримання **погоди** з API [OpenWeatherMap](https://openweathermap.org/)
- з'єднання з MQTT-брокером **HiveMQ Cloud**
- виведення інформації на **OLED-дисплей (SSD1306)**

## 🧠 Технології
- Wi-Fi з'єднання (ESP8266)
- HTTPS запит до `api.openweathermap.org`
- MQTT-з'єднання з `hivemq.cloud`
- I²C OLED дисплей (бібліотека `Adafruit_SSD1306`)
- ArduinoJson для парсингу JSON

## 📦 Компоненти
- ESP8266 (наприклад, NodeMCU)
- OLED SSD1306 128x64 (I2C)
- Інтернет
- Обліковий запис на [HiveMQ Cloud](https://console.hivemq.cloud/)

## 📡 Додатково
Топік MQTT: `emqx/esp8266`  
MQTT-брокер: `fb4fcf99cc2d416695328fd37cd76c33.s1.eu.hivemq.cloud`  
Погода завантажується кожні **60 секунд**.

## ⚙️ Як запустити
1. Встановити бібліотеки:
    - Adafruit SSD1306
    - Adafruit GFX
    - PubSubClient
    - ArduinoJson

При надсиланні повідомлення з MQTT Web Client:
На екрані з’явиться це повідомлення. "Привіт, ESP!"

2. У файлі `lab3.4.ino` вказати свій Wi-Fi і MQTT облікові дані.

3. Залити на ESP8266 через Arduino IDE.

4. Перевірити серіальний монітор (115200 baud).

## 🔗 Автор
**Пастух РОстислав, група ІР-23**  
Лабораторна робота №3 і №4
