
#include "ha_homeassistant.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
int HomeAssistant::sendGetRequest(String endpoint)
{
    if (!_ensureConnected())
        return 200; //! pretend it did something
    Serial.println("Preparing GET request NR to " + endpoint);
    _httpClient.begin(_host, _port, endpoint);
    _httpClient.addHeader("Authorization", getToken());
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
    _httpClient.addHeader("Authorization", getToken());
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
    _httpClient.addHeader("Authorization", getToken());
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
    _httpClient.addHeader("Authorization", getToken());
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
HomeAssistant::HomeAssistant(String host, int port){
    setHost(host);
    setPort(port);
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
String HomeAssistant::getClientId() {
    return String("http://" + WiFi.localIP().toString());
}

bool HomeAssistant::requestAccessToken(){
    if (refresh_token.length() == 0)
        return false;
        // return requestRefreshToken();

    Serial.println("Sending POST request for access");
    _httpClient.begin(_host, _port, "/auth/token");
    _httpClient.addHeader("content-type", "application/x-www-form-urlencoded");
    String body = String("grant_type=refresh_token&refresh_token=" + refresh_token + "&client_id=" + getClientId());
    int status_code = _httpClient.POST(body);
    if (status_code != 200)
        return false;
    JsonDocument response;
    deserializeJson(response, _httpClient.getStream());
    _httpClient.end();
    if (!response["access_token"].is<String>())
        return false;
    access_token = String("Bearer " + response["access_token"].as<String>());
    access_token_duration = response["expires_in"].as<int>();
    access_token_start_time = millis();
    return true;
};
String HomeAssistant::requestRefreshToken(String access_code){
    // if (code.length() == 0) 
    //     return requestRefreshToken();

    log_d("Sending POST request for refresh");
    _httpClient.begin(_host, _port, "/auth/token");
    _httpClient.addHeader("content-type", "application/x-www-form-urlencoded");
    String body = String("grant_type=authorization_code&code=" + access_code + "&client_id="+ getClientId());
    log_d("'%s', %s", body.c_str(), getClientId());
    int status_code = _httpClient.POST(body);
    if (status_code != 200)
        return "";
    JsonDocument response;
    deserializeJson(response, _httpClient.getStream());
    _httpClient.end();
    if (!response["refresh_token"].is<String>())
        return "";
    refresh_token = response["refresh_token"].as<String>();
    access_token = String("Bearer "+ response["access_token"].as<String>());
    access_token_duration = response["expires_in"].as<int>();
    access_token_start_time = millis();
    return refresh_token;
};
void HomeAssistant::reload() {

};
String HomeAssistant::getToken() {
    if (_use_refresh_tokens) {
        return getAccessToken();
    }else {
        return _token;
    };
};
void HomeAssistant::setRefreshToken(String rt) {
    refresh_token = rt;
    requestAccessToken();
}