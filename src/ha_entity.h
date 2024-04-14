#ifndef HA_ENTITY
#define HA_ENTITY
#include <Arduino.h>
#include <ArduinoJson.h>
#include "ha_homeassistant.h"
class Entity
{
private:
    unsigned long _lastUpdated;
    String _entity_id;
    String _domain;
    String _friendly_name;
    String _state = "unknown";
    void _updateState();
    

public:
    Entity(const String identifier);
    String getIdentifier();
    String getDomain();
    String getState();
    void updateIfRequired();
    String getFriendlyName();
    void updateStateFromJSON(JsonDocument data);
    bool turnOn();
    bool turnOff();
    bool toggle();
    // inline bool setBrightness(int brightness) { return false; };
};
#endif