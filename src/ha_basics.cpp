#ifndef HA_BASICS
#define HA_BASICS
#include "ha_homeassistant.h"

bool HomeAssistant::checkServerStatus()
{
    const JsonDocument response = sendGetRequestWithResponse("/api/");
    if (response["status_code"] != 200)
        // invalid response
        return false;
    if (response["message"] == "API running.")
        return true;
    return false;
}
void HomeAssistant::getDeviceList() {};

Entity *HomeAssistant::getEntityByIdentifier(String identifier)
{
    for (int i = 0; i < _definedEntities; i++)
    {
        if (_entities[i]->getIdentifier().equals(identifier))
        {
            return _entities[i];
        }
    }
    log_w("No Entity with identifier '%s' found", identifier.c_str());
    return nullptr;
}
Entity *HomeAssistant::getEntityByFriendlyName(String friendly_name)
{
    for (int i = 0; i < _definedEntities; i++)
    {
        if (_entities[i]->getFriendlyName().equals(friendly_name))
        {
            return _entities[i];
        }
    }
    log_w("No Entity with friendly_name '%s' found", friendly_name.c_str());
    return nullptr;
}
Entity *HomeAssistant::getEntityByIndex(int index)
{
    if (index < getNumEntities())
        return _entities[index];
    return nullptr;
}

bool HomeAssistant::addEntity(Entity *entity)
{
    if (_definedEntities >= HA_MAX_ENTITIES)
    {
        log_w("No empty space available");
        return false;
    }
    _entities[_definedEntities] = entity;
    log_d("Successfully added entity at position %d", _definedEntities);
    _definedEntities++;
    return true;
}
int HomeAssistant::triggerService(Entity *entity, String service, JsonDocument data)
{
    data["entity_id"] = entity->getIdentifier();
    int response = sendPostRequest(String("/api/services/" + entity->getDomain() + "/" + service), data);
    return response;
}
bool HomeAssistant::setToken(String token)
{
    _token = token;
    return true;
};
bool HomeAssistant::setHost(String host)
{
    _host = host;
    return true;
};
bool HomeAssistant::setPort(int port)
{
    _port = port;
    return true;
};
String HomeAssistant::getRefreshToken()
{
    return refresh_token;
}
String HomeAssistant::getAccessToken()
{
    int difference = (millis() - access_token_start_time) / 1000;
    if (abs(difference) > (access_token_duration - 10))
    { // has expired
        requestAccessToken();
    };
    return access_token;
}
bool HomeAssistant::setup(String token, String host, int port)
{
    if (not _use_refresh_tokens)
    {
        return setToken(token) && setHost(host) && setPort(port);
    }
    log_d("host:%s", host.c_str());
    return setHost(host) && setPort(port) && (requestRefreshToken(token).length() > 0);
};
bool HomeAssistant::isSetup()
{
    if (_use_refresh_tokens)
    {
        if (refresh_token.length() == 0)
            return false;
    }
    else
    {
        if (_token == "")
            return false;
    };
    if (_host == "")
        return false;
    if (_port == 0)
        return false;
    return true;
}
String HomeAssistant::getTimezone() {
    const JsonDocument response = sendGetRequestWithResponse("/api/config");

    if (not response.containsKey("time_zone"))
        // invalid response
        return "GMT0";

    return response["time_zone"].as<String>();
};

#endif