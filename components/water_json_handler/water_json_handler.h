
// components/water_json_handler/water_json_handler.h
#pragma once

#include "esphome.h"
#include "esphome/components/web_server_base/web_server_base.h"

#ifdef ARDUINO
  #include <ESPAsyncWebServer.h>
#endif

#include <cmath>

namespace esphome {
namespace water_json_handler {

class WaterJsonHandler : public Component, public AsyncWebHandler {
 public:
  // YAML -> codegen koppelt hier de sensoren aan
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) {
    this->flow_  = flow_sensor;   // l/min
    this->total_ = total_sensor;  // m³
  }

  void setup() override {
    if (web_server_base::global_web_server_base != nullptr) {
      web_server_base::global_web_server_base->add_handler(this);
      ESP_LOGI("water_json", "Registered /json.html handler");
    } else {
      ESP_LOGW("water_json", "Web server base not available");
    }
  }

  // Alleen GET /json.html accepteren
  bool canHandle(AsyncWebServerRequest *request) override {
    return request->method() == HTTP_GET && request->url() == "/json.html";
  }

  void handleRequest(AsyncWebServerRequest *request) override {
    float flow_lmin = 0.0f;
    float total_m3  = 0.0f;

    if (this->flow_  && std::isfinite(this->flow_->state))  flow_lmin = this->flow_->state;
    if (this->total_ && std::isfinite(this->total_->state)) total_m3  = this->total_->state;

    long total_liters = lroundf(total_m3 * 1000.0f);

    char body[128];
    // Strings zoals je vroeg (hele liters / hele l/min)
    snprintf(body, sizeof(body),
             "{\"waterflow\":\"%.0f\",\"waterquantity\":\"%ld\"}",
             flow_lmin, total_liters);

    request->send(200, "application/json", body);
  }

  bool isRequestHandlerTrivial() override { return true; }

 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};
};

}  // namespace water_json_handler
}  // namespace esphome
