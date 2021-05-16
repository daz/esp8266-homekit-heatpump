#include <homekit/homekit.h>
#include <homekit/characteristics.h>

homekit_characteristic_t cha_current_heating_cooling_state = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0);
homekit_characteristic_t cha_target_heating_cooling_state = HOMEKIT_CHARACTERISTIC_(TARGET_HEATING_COOLING_STATE, 0);
homekit_characteristic_t cha_current_temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);
homekit_characteristic_t cha_target_temperature = HOMEKIT_CHARACTERISTIC_(TARGET_TEMPERATURE, 0);
homekit_characteristic_t cha_temperature_display_units = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0);

void accessory_identify(homekit_value_t _value) {
  return;
}

homekit_accessory_t *accessories[] = {
  HOMEKIT_ACCESSORY(
    .id = 1,
    .category = homekit_accessory_category_air_conditioner,
    .services = (homekit_service_t*[]) {
      HOMEKIT_SERVICE(
        ACCESSORY_INFORMATION,
        .characteristics = (homekit_characteristic_t*[]) {
          HOMEKIT_CHARACTERISTIC(NAME, "Sensor Hub"),
          HOMEKIT_CHARACTERISTIC(MANUFACTURER, "ACCESSORY_MANUFACTURER"),
          HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "ACCESSORY_SN"),
          HOMEKIT_CHARACTERISTIC(MODEL, "ACCESSORY_MODEL"),
          HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.1"),
          HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
          NULL
        }
      ),
      HOMEKIT_SERVICE(
        THERMOSTAT,
        .primary = true,
        .characteristics = (homekit_characteristic_t*[]) {
          HOMEKIT_CHARACTERISTIC(NAME, "Air Conditioner"),
          &cha_current_heating_cooling_state,
          &cha_target_heating_cooling_state,
          &cha_current_temperature,
          &cha_target_temperature,
          &cha_temperature_display_units,
          NULL
        }
      ),
      NULL
    }
  ),
  NULL
};

homekit_server_config_t config = {
  .accessories = accessories,
  .password = "111-11-111"
};
