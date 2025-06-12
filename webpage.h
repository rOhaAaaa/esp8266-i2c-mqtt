#pragma once
#include <Arduino.h>

String generateWebPage(float temperature, int smokeLevel, bool alarmState, String eventLog[], int eventCount) {
  String html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  html += "<!DOCTYPE HTML><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Система пожежної безпеки</title>";

  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f2f2f2; text-align: center; padding: 20px; transition: background-color 0.5s; }";
  html += ".card, .log { background: white; padding: 20px; margin: 20px auto; border-radius: 12px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); max-width: 500px; }";
  html += ".alarm { color: red; font-size: 24px; font-weight: bold; }";
  html += ".normal { color: green; font-size: 22px; font-weight: bold; }";
  html += ".btn { background: red; color: white; padding: 12px 24px; font-size: 18px; border: none; border-radius: 8px; cursor: pointer; margin-top: 20px; }";
  html += ".btn:hover { background-color: darkred; }";
  html += ".filter { margin-bottom: 10px; padding: 10px; font-size: 16px; border-radius: 8px; }";
  html += "@keyframes blink { 0%{opacity:1;}50%{opacity:0.5;}100%{opacity:1;} }";
  html += ".blinking { animation: blink 1s infinite; }";
  html += ".log-content { text-align: left; white-space: pre-wrap; }";
  html += "</style>";

  html += "<script>";
  html += "let selectedFilter = 'all';";
  html += "function fetchData() {";
  html += "fetch('/data').then(response => response.json()).then(data => {";
  html += "document.getElementById('temp').innerHTML = data.temperature + ' °C';";
  html += "document.getElementById('smoke').innerHTML = data.smokeLevel;";
  html += "let status = document.getElementById('status');";
  html += "let body = document.body;";
  html += "let resetBtn = document.getElementById('resetBtn');";
  html += "let clearBtn = document.getElementById('clearBtn');";
  html += "if (data.alarm) { status.innerHTML = '🚨 ТРИВОГА!'; status.className = 'alarm blinking'; body.style.backgroundColor = '#ffe5e5'; resetBtn.style.display = 'inline-block'; }";
  html += "else { status.innerHTML = '✅ Все в нормі'; status.className = 'normal'; body.style.backgroundColor = '#f2f2f2'; resetBtn.style.display = 'none'; }";
  html += "updateEventLog(data.eventLog);";
  html += "});";
  html += "}";
  html += "function updateEventLog(events) {";
  html += "let log = document.getElementById('eventLog');";
  html += "let filtered = ''; let list = events.split('\\n');";
  html += "if (selectedFilter === 'last2') filtered = list.slice(0,2).join('<br>');";
  html += "else if (selectedFilter === 'last6') filtered = list.slice(0,6).join('<br>');";
  html += "else if (selectedFilter === 'last12') filtered = list.slice(0,12).join('<br>');";
  html += "else filtered = list.join('<br>');";
  html += "log.innerHTML = filtered;";
  html += "}";
  html += "function changeFilter(value) { selectedFilter = value; fetchData(); }";
  html += "function clearLog() { fetch('/clear').then(() => fetchData()); }";
  html += "setInterval(fetchData, 3000);";
  html += "</script>";

  html += "</head><body onload='fetchData()'>";

  html += "<h1>🏠 Система пожежної безпеки</h1>";

  html += "<div class='card'>";
  html += "<p><b>🌡 Температура:</b> <span id='temp'>...</span></p>";
  html += "<p><b>💨 Рівень диму:</b> <span id='smoke'>...</span></p>";
  html += "<p id='status' class='normal'>✅ Все в нормі</p>";
  html += "<form action='/reset' method='GET'><button id='resetBtn' class='btn' style='display:none;'>Скинути тривогу</button></form>";
  html += "</div>";

  html += "<div class='log'>";
  html += "<h3>📜 Історія подій:</h3>";
  html += "<select class='filter' onchange='changeFilter(this.value)'>";
  html += "<option value='all'>Всі події</option>";
  html += "<option value='last2'>Останні 2 події</option>";
  html += "<option value='last6'>Останні 6 подій</option>";
  html += "<option value='last12'>Останні 12 подій</option>";
  html += "</select>";
  html += "<div id='eventLog' class='log-content'>...</div>";
  html += "<br><button id='clearBtn' class='btn' onclick='clearLog()'>Очистити історію</button>";
  html += "</div>";

  html += "</body></html>";

  return html;
}

String generateJSON(float temperature, int smokeLevel, bool alarmState, String eventLog[], int eventCount) {
  String json = "{";
  json += "\"temperature\":" + String(temperature, 2) + ",";
  json += "\"smokeLevel\":" + String(smokeLevel) + ",";
  json += "\"alarm\":" + String(alarmState ? "true" : "false") + ",";
  json += "\"eventLog\":\"";
  for (int i = 0; i < eventCount; i++) {
    json += eventLog[i];
    if (i < eventCount - 1) json += "\\n";
  }
  json += "\"}";
  return json;
}
