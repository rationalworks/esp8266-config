#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H
#include "ArduinoJson.h"

namespace rationalworks
{
static  const char *default_ssid = "RW-Home";
static  const char *default_password = "homesecurity";
class WIFIManager
{
  private:
    char *m_ap_ssid;
    char *m_ap_password;
    char *sm_ssid;
    char *sm_password;
    // const char *default_ssid = "RW-Home";
    // const char *default_password = "homesecurity";
    

  public:
    WIFIManager();
    void start_ap();
    void start_sm();
    void set_ap_details(char *ap_ssid, char *ap_password);
    void scan_nw_json();
    DynamicJsonDocument* m_ssids_doc;
};
}

#endif
