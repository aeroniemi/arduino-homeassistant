#include "ha_entity.h"
#include "entities/list_entities.h"
Entity::Entity(String identifier)
{
    _entity_id = identifier;
    _domain = identifier.substring(0, identifier.indexOf("."));
    log_d("Creating entity with name '%s' and domain '%s", _entity_id.c_str(), _domain.c_str());
};
String Entity::getIdentifier()
{
    return _entity_id;
}
String Entity::getDomain()
{
    return _domain;
}
String Entity::getFriendlyName()
{
    return _friendly_name;
}
String Entity::getState()
{
    updateIfRequired();
    return _state;
}
void Entity::updateIfRequired()
{
    if (millis() - _lastUpdated < 5 * 1000)
        return;
    _updateState();
}
void Entity::_updateState()
{
    JsonDocument response = ha->sendGetRequestWithResponse(String("/api/states/" + _entity_id));
    if (response["status_code"] != 200)
    {
        log_e("Failed to update state: code %d", response["status_code"]);
    }
    updateStateFromJSON(response);
}
void Entity::updateStateFromJSON(JsonDocument data)
{
    _state = data["state"].as<String>();
    _friendly_name = data["attributes"]["friendly_name"].as<String>();
    log_d("FN: %s", _friendly_name);
}
int HomeAssistant::getNumEntities()
{
    return _definedEntities;
}
void HomeAssistant::createEntities()
{
    if (_networking_enabled)
    {
        _httpClient.begin(_host, _port, "/api/states");
        _httpClient.addHeader("Authorization", _token);
        int status_code = _httpClient.GET();
        if (status_code != 200)
        {
            log_e("/api/states returned error code %d", status_code);
            return;
        }
        log_d("Network request underway, code 200");
        JsonDocument entity_json;
        WiFiClient http_stream = _httpClient.getStream();
        http_stream.find("[");
        do
        {
            deserializeJson(entity_json, http_stream);
            const String entity_id = entity_json["entity_id"];
            if (entity_id.startsWith("light."))
            {
                log_d("Light(%s)", entity_id.c_str());
                if (!addEntity(new Light(entity_id)))
                    break;
            }
            else if (entity_id.startsWith("switch."))
            {
                log_d("Switch(%s)", entity_id.c_str());
                if (!addEntity(new Switch(entity_id)))
                    break;
            }
            //! cleanup
        } while (http_stream.findUntil(",", "]"));
        _httpClient.end();
    }
}
void HomeAssistant::updateAllStates()
{
    if (_networking_enabled)
    {
        _httpClient.begin(_host, _port, "/api/states");
        _httpClient.addHeader("Authorization", _token);
        int status_code = _httpClient.GET();
        if (status_code != 200)
        {
            log_e("/api/states returned error code %d", status_code);
            return;
        }
        log_d("Network request underway, code 200");
        JsonDocument entity_json;
        WiFiClient http_stream = _httpClient.getStream();
        http_stream.find("[");
        do
        {
            deserializeJson(entity_json, http_stream);
            Entity *entity = getEntityByIdentifier(entity_json["entity_id"].as<String>());
            if (entity != nullptr)
            {
                entity->updateStateFromJSON(entity_json);
            }
            //! cleanup
        } while (http_stream.findUntil(",", "]"));
        _httpClient.end();
    }
}
Entity *HomeAssistant::getActiveEntity()
{
    return _activeEntity;
}
void HomeAssistant::setActiveEntity(Entity *entity)
{
    _activeEntity = entity;
}
// void HomeAssistant::updateStates() {
//     Stream http_stream = sendGetRequestStream("/api/states");
//     JsonDocument entity_json;
//     http_stream.find("[");
//     do
//     {
//         deserializeJson(entity_json, http_stream);
//         const String entity_id = entity_json["entity_id"];
//         Entity *found = getEntityByIdentifier(entity_id);
//         if (found != nullptr) {
//             found->updateStateFromJSON(entity_json);
//         }
//         //!cleanup
//     } while (http_stream.findUntil(",", "]"));
// }

bool Entity::turnOn()
{
    return ha->triggerService(this, "turn_on") == 200;
}
bool Entity::turnOff()
{
    return ha->triggerService(this, "turn_off") == 200;
}
bool Entity::toggle()
{
    return ha->triggerService(this, "toggle") == 200;
}