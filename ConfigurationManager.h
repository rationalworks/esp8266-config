#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include "ArduinoJson.h"
namespace rationalworks
{
class ConfigurationManager
{
  private:
    const String CONFIG_FILE = "/config.js";
    DynamicJsonDocument* m_setting;

  public:
    ConfigurationManager(int size);
    ConfigurationManager();
    void save_configuration();
    void load_configuration();
    bool is_configured();
    void set_configured();
    void reset_configured();
};
}

#endif
