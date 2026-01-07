
#pragma once

#include "esphome.h"
#include "esphome/components/web_server_base/web_server_base.h"

// ESPHome uses USE_ARDUINO to gate the Arduino/ESPAsyncWebServer path
#if defined(USE_ARDUINO)
  #include <ESPAsyncWebServer.h>
#endif

#include <cmath>

namespace esphome {
namespace water_json_handler {

// Arduino/ESP8266 (ESPAsyncWebServer) implementation
#if defined(USE_ARDUINO)

class WaterJsonHandler : public Component, public ::AsyncWebHandler {
 public:
  // Link your sensors (flow in l/min, total in m³)
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) {
    this->flow_  = flow_sensor;
    this->total_ = total_sensor;
  }

  void setup() override {
    if (web_server_base::global_web_server_base != nullptr) {
      web_server_base::global_web_server_base->add_handler(this);
      ESP_LOGI("water_json", "Registered /json.html handler (Arduino)");
    } else {
      ESP_LOGW("water_json", "Web server base not available");
    }
  }

  // NOTE: don't mark override — some cores differ slightly; matching name/signature is enough
  bool canHandle(::AsyncWebServerRequest *request) {
    return request && request->method() == HTTP_GET && request->url() == "/json.html";
  }

  void handleRequest(::AsyncWebServerRequest *request) {
    float flow_lmin = 0.0f;
    float total_m3  = 0.0f;

    if (this->flow_  && std::isfinite(this->flow_->state))  flow_lmin = this->flow_->state;
    if (this->total_ && std::isfinite(this->total_->state)) total_m3  = this->total_->state;

    long total_liters = lroundf(total_m3 * 1000.0f);

    char body[128];
    snprintf(body, sizeof(body),
             "{\"waterflow\":\"%.0f\",\"waterquantity\":\"%ld\"}",
             flow_lmin, total_liters);

    request->send(200, "application/json", body);
  }

  bool isRequestHandlerTrivial() { return true; }

 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};
};

// Non‑Arduino (ESP‑IDF) fallback: just register component and log a message.
// (Custom endpoints need a different backend; your node is ESP8266, so this path won’t be used.)
#else

class WaterJsonHandler : public Component {
 public:
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) {
    this->flow_  = flow_sensor;
    this->total_ = total_sensor;
  }

  void setup() override {
    ESP_LOGW("water_json", "Custom /json.html endpoint not available on non‑Arduino backend.");
  }

 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};
};

#endif  // USE_ARDUINO

}  // namespace water_json_handler
}  // namespace esphome
