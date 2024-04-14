#ifndef HA_HOMEASSISTANT
#define HA_HOMEASSISTANT

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "ha_entity.h"

#ifndef HA_MAX_ENTITIES
#define HA_MAX_ENTITIES 30
#endif
class Entity;
const JsonDocument empty_json;
class HomeAssistant
{
private:
    WiFiClient _wifiClient;
    HTTPClient _httpClient;
    String _token;
    String _host;
    int _port;
    bool _ensureConnected();
    Entity *_entities[HA_MAX_ENTITIES];
    int _definedEntities = 0;
    Entity *_activeEntity;
    bool _networking_enabled = true;

public:
    HomeAssistant();
    HomeAssistant(String token, String host, int port);
    bool setToken(String token);
    bool setHost(String host);
    bool setPort(int port);
    void getDeviceList();
    int getNumEntities();
    bool isConnected();
    bool checkServerStatus();
    int triggerService(Entity *entity, String service, JsonDocument data = JsonDocument());
    int sendGetRequest(String endpoint);
    JsonDocument sendGetRequestWithResponse(String endpoint);
    int sendPostRequest(String endpoint, JsonDocument body = JsonDocument());
    JsonDocument sendPostRequestWithResponse(String endpoint, JsonDocument body = JsonDocument());
    Entity *getEntityByIdentifier(String identifier);
    Entity *getEntityByFriendlyName(String friendly_name);
    Entity *getEntityByIndex(int index);
    bool addEntity(Entity *entity);
    void updateAllStates();
    void createEntities();
    Entity *getActiveEntity();
    void setActiveEntity(Entity *entity);
    bool setup(String token, String host, int port);
    bool isSetup();
    void init();
};
extern HomeAssistant *ha;

#endif