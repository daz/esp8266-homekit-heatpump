#define HOSTNAME "aircon1"
#include <WiFiSetup.h>

#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <DaikinHeatpumpARC480A14IR.h>

#define IR_PIN D8

void homekitSetup();
void homekitLoop();

void setPowerOn();
void setPowerOff();
void sendIR();

IRSenderBitBang irSender(IR_PIN);
DaikinHeatpumpARC480A14IR *heatpump;

struct {
  uint8_t power  = POWER_OFF;
  uint8_t mode   = MODE_COOL;
  uint8_t fan    = FAN_AUTO;
  uint8_t vswing = VDIR_UP;
  uint8_t hswing = VDIR_UP;
  float temp     = 25.0;
} State;

void setup() {
  Serial.begin(115200);

  initWiFi();
  initOTA();

  initTelnetDebug();
  heatpump = new DaikinHeatpumpARC480A14IR();

  // to remove the previous HomeKit pairing storage
  // homekit_storage_reset();
  homekitSetup();
}

void loop() {
  ArduinoOTA.handle();
  Debug.handle();
  homekitLoop();
  delay(10);
}

extern "C" homekit_server_config_t config;

extern "C" homekit_characteristic_t cha_current_heating_cooling_state;
extern "C" homekit_characteristic_t cha_target_heating_cooling_state;
extern "C" homekit_characteristic_t cha_current_temperature;
extern "C" homekit_characteristic_t cha_target_temperature;
extern "C" homekit_characteristic_t cha_temperature_display_units;
extern "C" homekit_characteristic_t cha_current_relative_humidiy;

void set_target_heating_cooling_state(const homekit_value_t value) {
  cha_current_heating_cooling_state.value.uint8_value = value.uint8_value;
  setPowerOn();

  switch (value.uint8_value) {
    case 0:
      setPowerOff();
      break;
    case 1:
      State.mode = MODE_HEAT;
      break;
    case 2:
      State.mode = MODE_COOL;
      break;
    case 3:
      State.mode = MODE_AUTO;
      break;
  }

  sendIR();
}

void set_target_temperature(const homekit_value_t value) {
  cha_current_temperature.value.float_value = value.float_value;
  State.temp = value.float_value;
  sendIR();
}

homekit_value_t get_target_heating_cooling_state() {
  uint8_t val = 0;

  switch (State.mode) {
    case MODE_HEAT:
      val = 1;
      break;
    case MODE_COOL:
      val = 2;
      break;
    case MODE_AUTO:
      val = 3;
      break;
  }

  if (State.power == POWER_OFF) {
    val = 0;
  }

  return HOMEKIT_UINT8_CPP(val);
}

homekit_value_t get_current_heating_cooling_state() {
  return get_target_heating_cooling_state();
}

homekit_value_t get_target_temperature() {
  return HOMEKIT_FLOAT_CPP(State.temp);
}

void setPowerOn() {
  State.power = POWER_ON;
}

void setPowerOff() {
  State.power = POWER_OFF;
}

void sendIR() {
  heatpump->send(irSender, State.power, State.mode, State.fan, State.temp, State.vswing, State.hswing, 0, 0, 0, 0, 0);
}

void homekitSetup() {
  cha_target_heating_cooling_state.setter = set_target_heating_cooling_state;
  cha_target_heating_cooling_state.getter = get_target_heating_cooling_state;
  cha_current_heating_cooling_state.getter = get_current_heating_cooling_state;
  cha_target_temperature.setter = set_target_temperature;
  cha_target_temperature.getter = get_target_temperature;

  arduino_homekit_setup(&config);
}

void homekitLoop() {
  arduino_homekit_loop();
}
