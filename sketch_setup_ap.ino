#include <ESP8266WiFi.h>
#include <WiFiClient.h>


#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>

#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "WIFIManager.h"
#include "ConfigurationManager.h"
#include "RWManager.h"

#ifndef APSSID
#define APSSID "RW-Home"
#define APPSK  "homesecurity"
#endif


/* Set these to your desired credentials. */
const char *ssid = "RW-Home";
const char *password = "homesecurity";

AsyncWebServer server(80);

//rationalworks::ConfigurationManager cm(1000);
rationalworks::RWManager* rwm = NULL;

DynamicJsonDocument setting(1024);

#define LED 2 //Define blinking LED pin

void scan_nw_json(DynamicJsonDocument& ssids_doc) {
  JsonArray data = ssids_doc.createNestedArray("ssids");
  int n = WiFi.scanComplete();
  if (n == -2) {
    WiFi.scanNetworks(false, true);
  } else if (n) {
    for (int i = 0; i < n; ++i) {
      data.add(WiFi.SSID(i));
      Serial.print("Added wifi ssid to results ");
      Serial.println(WiFi.SSID(i));
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(false, true);
    }
  }

}

void scan_nw() {
  int numberOfNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numberOfNetworks; i++) {
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
  }
}

void setup_server()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Got request");
    request->send(SPIFFS, "/index.html", "text/html");
  });

  Dir dir = SPIFFS.openDir("/");
  // or Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    String filename = dir.fileName();
    char HTML_EXTENSION[] = "html";
    char CSS_EXTENSION[] = "css";
    char JS_EXTENSION[] = "js";
    if (0 == strcmp(get_filename_ext(filename.c_str()), HTML_EXTENSION) ||
        0 == strcmp(get_filename_ext(filename.c_str()), CSS_EXTENSION) ||
        0 == strcmp(get_filename_ext(filename.c_str()), JS_EXTENSION))
    {
      Serial.print("File served ");
      Serial.println(filename);
      server.serveStatic(filename.c_str(), SPIFFS, filename.c_str());
    } else
    {
      Serial.print("Can't serve ");
      Serial.println(filename);
    }
  }

  server.on("/api/getssid", HTTP_GET, handle_get_ssid);

  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (request->url() == "/api/save/setting/nw") {
      if (!handle_save_nw(request, data))
      {
        request->send(500, "text/plain", "{'status':'Fail'}");
      } else
      {
        request->send(200, "text/plain", "{'status':'Success'}");
      }
    }
  });

  server.on("/api/finish", HTTP_GET, [](AsyncWebServerRequest * request) {
    setting["configured"] = 1;
    save_config();
    request->send(200, "text/plain", "{'status':'Success'}");
    ESP.restart();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void start_ap()
{
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  WiFi.scanNetworks(false, true);

}

void connect_ap()
{
  const char* ssid     = setting["selected_ssid"];        // The SSID (name) of the Wi-Fi network you want to connect to
  const char* password = setting["ssid_password"];    // The password of the Wi-Fi network

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void setup() {
  delay(1000);
  Serial.begin(9600);

  pinMode(LED, OUTPUT); // Initialize the LED pin as an output

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

   rwm = new rationalworks::RWManager(ssid, password, 1024, 8080);
   rwm->initilize();
  digitalWrite(LED, LOW); // Turn the LED on (Note that LOW is the voltage level)
  delay(1000); // Wait for a second
  digitalWrite(LED, HIGH); // Turn the LED off by making the voltage HIGH
  delay(1000); // Wait for two seconds
  /*
    load_config();
    if (setting["configured"] == 1)
    {
      Serial.println("Network is already confgured");
      connect_ap();
    } else
    {
      Serial.println("Network is not confgured");
      start_ap();
      setup_server();
    }



    send_message();
  */
}


const char *get_filename_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename) return "";
  return dot + 1;
}


void handle_get_ssid(AsyncWebServerRequest *request)
{

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument doc(1024);
  scan_nw_json(doc);
  serializeJson(doc, *response);
  request->send(response);
}


bool handle_save_nw(AsyncWebServerRequest *request, uint8_t *datas) {
  DynamicJsonDocument selected_nw_setting(1024);
  deserializeJson(selected_nw_setting, (const char*)datas);
  setting["selected_ssid"] = selected_nw_setting["selected_ssid"];
  Serial.println("Got SSID ");
  //Serial.println(setting["selected_ssid"]);
  setting["ssid_password"] = selected_nw_setting["ssid_password"];
  Serial.println("Got SSID password ");
  //Serial.println(setting["ssid_password"]);
  save_config();
  return 0;
}

void save_config()
{
  Serial.println("About to save configurations");
  File file = SPIFFS.open("/config.js", "w");
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  serializeJson(setting, file);
  Serial.println("Setting saved to file");
  file.close();
  Serial.println("Setting file is closed");
}

void load_config()
{
  Serial.println("Loading configurations");
  File file = SPIFFS.open("/config.js", "r");
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  deserializeJson(setting, file);
  Serial.println("Read configuration from file");
  file.close();
  Serial.println("Setting file is closed");
}

void send_message()
{
  Serial.println("Sending test message to slack");
  int i = 0;
  while (WiFi.localIP().toString() == "0.0.0.0") { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println("WIF is connected");

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    callhttps();

  } else {

    Serial.println("Error in WiFi connection");

  }
}


void callhttps() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  //client->setFingerprint(fingerprint);
  client->setInsecure();

  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  if (https.begin(*client, "https://slack.com/api/chat.postMessage")) {  // HTTPS

    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header
    https.addHeader("Content-Type", "application/json");
    https.addHeader("Authorization", "Bearer xoxb-1281118821536-1273491910855-DyB6LGycmSyKla9teKfKXApf");
    int httpCode = https.POST("{\"channel\": \"general\",\"text\": \"Hello from ESP 8266\"}");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        //Serial.println(payload);
      }
    } else {
      //Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
}

void loop() {

}
