#include <FS.h>
#include "ConfigurationManager.h"

namespace rationalworks
{
// constructor
ConfigurationManager::ConfigurationManager(int size)
{
  Serial.printf("ConfigurationManager constructor initilizing\n");
  DynamicJsonDocument setting(size);
  m_setting = &setting;
  while (!SPIFFS.begin()) { // Wait forSPIFFS to initilize
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.printf("ConfigurationManager initilized\n");
}

ConfigurationManager::ConfigurationManager()
{}

// member function
void ConfigurationManager::save_configuration()
{
  Serial.println("About to save configurations");
  File file = SPIFFS.open(CONFIG_FILE, "w");
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  serializeJson(*m_setting, file);
  Serial.println("Setting saved to file");
  file.close();
  Serial.println("Setting file is closed");
}

void ConfigurationManager::load_configuration()
{
  Serial.println("Loading configurations");
  File file = SPIFFS.open(CONFIG_FILE, "r");
  Serial.println("Opening configuration file for reading");
  if (!file) {
    Serial.println("Error opening file for reading");
    return;
  }
  deserializeJson(*m_setting, file);
  Serial.println("Read configuration from file");
  file.close();
  Serial.println("Setting file is closed");
}

bool ConfigurationManager::is_configured()
{
  Serial.printf("Invoking ConfigurationManager::is_configured\n");
  //Serial.println((*m_setting)["configured"]);
  if (!((*m_setting)["configured"]) && ((*m_setting)["configured"] == "1"))
  {
    Serial.printf("ConfigurationManager::is_configured return true\n");
    return true;
  } else
  {
    Serial.printf("ConfigurationManager::is_configured return false\n");
    return false;
  }
}

void ConfigurationManager::set_configured()
{
  (*m_setting)["configured"] = "1";
  save_configuration();
}

void ConfigurationManager::reset_configured()
{
  (*m_setting)["configured"] = "0";
  save_configuration();
}


}
