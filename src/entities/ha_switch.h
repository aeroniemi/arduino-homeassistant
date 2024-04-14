#ifndef HA_SWITCH
#define HA_SWITCH
#include "../ha_entity.h"
class Switch : public Entity
{
public:
    using Entity::Entity;
    void turnOn();
    void turnOff();
    void toggle();
    bool getState();
};
#endif