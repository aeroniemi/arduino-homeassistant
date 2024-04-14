
#include "ha_homeassistant.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <aero_error_handling.h>

UIError error_no_home_assistant(F("Cannot connect to HomeAssistant"));
UIError error_auth_home_assistant(F("Authentication with HomeAssistant failed - check token"));
UIInfo info_example(F("This is an example of an info message, hello!"), F("Go Team!"));

int HomeAssistant::sendGetRequest(String endpoint)
{
    if (!_ensureConnected())
        return 200; //! pretend it did something
    Serial.println("Preparing GET request NR to " + endpoint);
    _httpClient.begin(_host, _port, endpoint);
    _httpClient.addHeader("Authorization", _token);
    Serial.println("Sending GET request to " + endpoint);
    int status_code = _httpClient.GET();
    log_d("GET request dealt with");
    _httpClient.end();
    log_d("GET request completed with status code: %d", status_code);
    return status_code;
}
JsonDocument HomeAssistant::sendGetRequestWithResponse(String endpoint)
{
    if (!_ensureConnected())
    {
        JsonDocument response;
        response["status_code"] = 200;
        return response;
    }
    Serial.println("Sending GET request WR to " + endpoint);
    _httpClient.begin(_host, _port, endpoint);
    _httpClient.addHeader("Authorization", _token);
    int status_code = _httpClient.GET();
    JsonDocument response;
    deserializeJson(response, _httpClient.getStream());
    _httpClient.end();
    response["status_code"] = status_code;
    Serial.println("Status: " + status_code);

    return response;
}
int HomeAssistant::sendPostRequest(String endpoint, JsonDocument body)
{
    if (!_ensureConnected())
        return 200; //! pretend it did something
    Serial.println("Sending POST request to " + endpoint);
    _httpClient.begin(_host, _port, endpoint);
    _httpClient.addHeader("Authorization", _token);
    _httpClient.addHeader("content-type", "application/json");
    String json;
    serializeJson(body, json);
    int status_code = _httpClient.POST(json);
    _httpClient.end();
    Serial.println("Status: " + status_code);
    return status_code;
}
JsonDocument HomeAssistant::sendPostRequestWithResponse(String endpoint, JsonDocument body)
{
    if (!_ensureConnected())
    {
        JsonDocument response;
        response["status_code"] = 200;
        return response;
    }
    Serial.println("Sending POST request to " + endpoint);
    _httpClient.begin(_host, _port, endpoint);
    _httpClient.addHeader("Authorization", _token);
    _httpClient.addHeader("content-type", "application/json");
    String json;
    serializeJson(body, json);
    int status_code = _httpClient.POST(json);
    JsonDocument response;
    deserializeJson(response, _httpClient.getStream());
    _httpClient.end();
    response["status_code"] = status_code;
    Serial.println("Status: " + status_code);
    return response;
}
void HomeAssistant::init() {
    if (!isConnected())
    {
        _networking_enabled = false;
    }
    _httpClient.useHTTP10(true);
    // _httpClient.setReuse(true);
}
HomeAssistant::HomeAssistant(){};
HomeAssistant::HomeAssistant(String token, String host, int port){
    setup(token, host, port);
};
bool HomeAssistant::isConnected()
{
    return WiFi.status() == WL_CONNECTED && _networking_enabled;
}
bool HomeAssistant::_ensureConnected()
{
    if (isConnected())
        return true;
    //! do something
    return false;
}