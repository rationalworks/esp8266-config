#include <ESP8266WiFi.h>
#include "WIFIManager.h"

namespace rationalworks
{
// constructor
WIFIManager::WIFIManager()
{
    DynamicJsonDocument ssids_doc(1024);
    m_ssids_doc = &ssids_doc;
}

// member function
void WIFIManager::set_ap_details(char *ap_ssid, char *ap_password)
{
  m_ap_ssid = ap_ssid;
  m_ap_password = ap_password;
}

void WIFIManager::start_ap()
{
  Serial.println("Configuring access point...");

    if (!m_ap_ssid && !m_ap_password)
    {
    Serial.printf("Setting access point with SSID %s\n", m_ap_ssid);
    Serial.printf("Setting access point with password %s\n", m_ap_password);
    WiFi.softAP(m_ap_ssid, m_ap_password);
    } else
    {
    Serial.println("Starting AP with default credentials");
    Serial.printf("Setting access point with SSID %s\n", default_ssid);
    Serial.printf("Setting access point with password %s\n", default_password);
    WiFi.softAP(default_ssid, default_password);
    }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  WiFi.scanNetworks(false, true);
  Serial.println("Started access point...");
}

void WIFIManager::start_sm()
{
  if (!sm_ssid && !sm_password)
  {
    WiFi.begin(default_ssid, default_password);             // Connect to the network
  } else
  {
    WiFi.begin(sm_ssid, sm_password);
  }
  Serial.print("Connecting to ");
  Serial.print(sm_ssid); Serial.println(" ...");
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

void WIFIManager::scan_nw_json() {
  m_ssids_doc->clear();
  JsonArray data = m_ssids_doc->createNestedArray("ssids");
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
}
