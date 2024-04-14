#ifndef HA_LIGHT
#define HA_LIGHT
#include "../ha_entity.h"
#include "../ha_homeassistant.h"
class Light : public Entity
{
private:
    bool _mode_brightness;
    bool _mode_color_temp;
    bool _mode_rgb;
    bool _mode_rgbw;

public:
    using Entity::Entity;
    bool getState();
    inline bool setBrightness(int brightness)
    {
        JsonDocument request;
        request["brightness"] = brightness;
        return ha->triggerService(this, "turn_on", request) == 200;
    };
    int getBrightness();
    void getColor();
    void setColor();
    bool supportsBrightness() { return _mode_brightness; };
    bool supportsRGB() { return _mode_rgb or _mode_rgbw; };
    bool supportsColorTemp() { return _mode_color_temp; };
};
#endif