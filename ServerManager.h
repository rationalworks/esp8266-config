#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

namespace rationalworks
{

class ServerMapping
{
  public:
    char* path;
    void (*handler_method)(AsyncWebServerRequest*);
};

class ServerManager
{
  private:
    int m_port;
    const char* get_filename_ext(const char *filename);
    rationalworks::ServerMapping* m_server_mapping;


  public:
    ServerManager(int c_port);
    ServerManager();
    void serve_static();
    void start_server();
    void set_routes();
    AsyncWebServer* server;
};
}
#endif
