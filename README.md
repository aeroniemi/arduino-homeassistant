# Library for accessing Home Assistant from Arduino
For [CircleHome](https://github.com/aeroniemi/circlehome) I needed to access various Home Asssistant API endpoints using Arduino. This library contains the code used for that. Specifically this library allows you to list all of the entities in Home Assistant, so you don't have to add each one manually if they change.

It supports:
* HA Auth Flow (see [HA Docs](https://developers.home-assistant.io/docs/auth_api/))
* Entities:
    * Light
    * Switch
    * Listing all entities of these types

It's fairly trivial to add extra entity types - see ``/src/entities/``.
