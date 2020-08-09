#include <ESP8266WiFi.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "RWManager.h"
#include "ServerManager.h"

namespace rationalworks
{
// constructor
RWManager::RWManager()
{}

RWManager::RWManager(const char *ap_ssid , const char *ap_password, int c_size, int c_port)
{
  Serial.println("RWManager constructor initilizing");
  m_cm = new rationalworks::ConfigurationManager(c_size);
  m_nw = new rationalworks::WIFIManager();
  m_port = c_port;
  Serial.println("RWManager constructor initilized");

}

// member function
void RWManager::initilize()
{
  Serial.printf("RWManager Initilizing....\n");
  m_cm->load_configuration();
  bool is_configured = m_cm->is_configured();
  if (is_configured)
  {
    Serial.printf("RWManager connecting to local wifi\n");
    m_nw->start_sm();
  } else {
    Serial.printf("RWManager starting access point\n");
    m_nw->start_ap();
    m_server = new rationalworks::ServerManager(m_port);
    m_server->serve_static();
    //m_server->set_routes();

    

    /*
      m_server->server->on("/api/getssid", HTTP_GET, [this](AsyncWebServerRequest * request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        this->m_nw->scan_nw_json();
        DynamicJsonDocument* ssids = this->m_nw->m_ssids_doc;
        serializeJson(*ssids, *response);
        request->send(response);
      });

        m_server->server->on("/api/finish", HTTP_GET, [](AsyncWebServerRequest * request) {
          request->send(200, "text/plain", "{'status':'Success'}");
          ESP.restart();
        });
    */
    m_server->start_server();
  }
}

}
