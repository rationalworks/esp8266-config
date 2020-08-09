#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "ServerManager.h"

namespace rationalworks
{
// constructor
ServerManager::ServerManager(int c_port)
{
  Serial.println("ServerManager constructor initilizing");
  m_port = c_port;
  server = new AsyncWebServer(m_port);
  rationalworks::ServerMapping server_mapping[5];
  m_server_mapping = server_mapping;
  Serial.printf("Webserver started on port %d\n", c_port);
  Serial.println("ServerManager constructor initilized");
}

ServerManager::ServerManager()
{}

void ServerManager::serve_static()
{
  Serial.println("ServerManager setting up to serve index.html");
  server->on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Got request for /");
    request->send(SPIFFS, "/index.html", "text/html");
  });

  Serial.println("ServerManager setting up to serve other static files");
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
      Serial.print("Serving file ");
      Serial.println(filename);
      server->serveStatic(filename.c_str(), SPIFFS, filename.c_str());
    } else
    {
      Serial.print("Can't serve ");
      Serial.println(filename);
    }
  }
  Serial.printf("HTTP server started on port %d", m_port);
}

void ServerManager::start_server()
{
  server->begin();
}

void ServerManager::set_routes()
{
  
}

const char* ServerManager::get_filename_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename) return "";
  return dot + 1;
}



}
