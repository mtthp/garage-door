/**
 * Garage Door Controller
 *
 * Control a servo attached to the server
 * and serve an API to manage it
 * inspired by https://github.com/julienrat/petitbot/tree/master/code_arduino/petitbot_v2
 *
 * @author Matthieu Petit <mpetit@citizencam.eu>
 * @date 01/11/2017
 */

#include <ESP8266WiFi.h>          // ESP8266 Core WiFi Library
#include <ESP8266WebServer.h>     // Web Server to serve the API
#include <Servo.h>
#include "user_config.h"          // user configuration file

Servo servo;
const int pin_servo = SERVO_PIN;
int current_angle = 0;
int current_speed = 0; // currently useless
int modified = 0;

ESP8266WebServer server(HTTP_PORT);
const char *http_methods[] = {"UNKNOWN", "GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS"};

void setup() {
  Serial.begin(SERIAL_RATE);
  Serial.printf("\n[%d] Booting...", millis());

  // servo init
  servo.attach(pin_servo);
  servo.write(current_angle);

  // wifi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("\n[%d] (WiFi) Connecting to %s", millis(), WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("\n[%d] (WiFi) Connected !", millis());

  // http server setup
  setupApi();
  server.begin();
  Serial.printf("\n[%d] (HTTP) Ready at http://", millis());
  Serial.print(WiFi.localIP());
  Serial.print(":" + String(HTTP_PORT));
}

void loop() {
  server.handleClient();
}

/**
 * Response the current status of the server
 */
void getStatus() {
  String message = "{";
  message += "\"angle\": " + String(current_angle) + ",";
  //message += "\"speed\": " + String(current_speed) + ",";
  message += "\"uptime\": " + String(millis()) + ",";
  message += "\"modified\": " + String(modified);
  message += "}";
  server.send(200, "text/json", message);
}

/**
 * Reponse an error message
 */
void handleNotFound() {
  logAccess();
  String message = "{";
  message += "\"status\": " + String(404) + ",";
  message += "\"message\": \"Resource not found\",";
  message += "\"uri\": \"" + server.uri() + "\",";
  message += "\"method\": \"" + String(http_methods[server.method()]) + "\",";
  message += "\"arguments\": {";
  for (uint8_t i=0; i<server.args(); i++){
    message += "\"" + server.argName(i) + "\": \"" + server.arg(i) + "\"";
    if (i < server.args() - 1) {
      message += ",";
    }
  }
  message += "},";
  message += "\"uptime\": " + String(millis());
  message += "}";
  server.send(404, "text/json", message);
}

/**
 * Log access to the HTTP server
 */
void logAccess() {
  Serial.printf("\n[%d] (HTTP) ", millis());
  Serial.print(server.client().remoteIP().toString());
  Serial.print(" >> ");
  Serial.print(http_methods[server.method()]);
  Serial.print(" " + server.uri());
  Serial.print(" {");
  String message;
  for (uint8_t i=0; i<server.args(); i++){
    message += "\"" + server.argName(i) + "\": \"" + server.arg(i) + "\"";
    if (i < server.args() - 1) {
      message += ",";
    }
  }
  Serial.print(message);
  Serial.print("}");
}

/**
 * Setup the routes for the API
 */
void setupApi() {
  server.on("/", HTTP_GET, []() {
    logAccess();
    getStatus();
  });

  server.on("/toggle", HTTP_GET, []() {
    logAccess();
    current_angle = 0;
    servo.write(current_angle);
    delay(500);
    current_angle = SERVO_TOGGLE_ANGLE;
    servo.write(current_angle);
    delay(500);
    current_angle = 0;
    servo.write(current_angle);
    modified = millis();
    getStatus();
  });

  server.on("/", HTTP_POST, []() {
    logAccess();
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i) == "angle") {
        current_angle = server.arg(i).toInt();
        servo.write(current_angle);
        modified = millis();
      }
      if (server.argName(i) == "speed") {
        current_speed = server.arg(i).toInt();
        modified = millis();
      }
    }
    getStatus();
  });

  server.onNotFound(handleNotFound);
}

