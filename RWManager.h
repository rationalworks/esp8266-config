#ifndef RWMANAGER_H
#define RWMANAGER_HNAGER_H

#include "ConfigurationManager.h"
#include "WIFIManager.h"
#include "ServerManager.h"

namespace rationalworks
{
class RWManager
{
  private:
    const char *ap_ssid = "RW-Home";
    const  char *ap_password = "homesecurity";
    ConfigurationManager* m_cm;
    WIFIManager* m_nw;
    ServerManager* m_server;
    int m_port;

  public:
    RWManager(const char *ap_ssid , const char *ap_password, int c_size, int c_port);
    RWManager();
    void initilize();
    void set_default_api_mapping();
    void set_custom_api_mapping();
};
}

#endif
