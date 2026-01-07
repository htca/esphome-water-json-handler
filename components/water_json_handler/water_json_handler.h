
#pragma once
#include "esphome.h"
#include "esphome/components/web_server_base/web_server_base.h"
#if defined(USE_ARDUINO)
  #include <ESPAsyncWebServer.h>
#endif
#include <cmath>

namespace esphome {
namespace water_json_handler {

#if defined(USE_ARDUINO)
class WaterJsonHandler : public Component, public ::AsyncWebHandler {
 public:
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) {
    this->flow_  = flow_sensor;
    this->total_ = total_sensor;
  }
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }
  void setup() override { this->attach_or_retry_(); }

  bool canHandle(::AsyncWebServerRequest *request) {
    return request && request->method() == HTTP_GET && request->url() == "/json.html";
  }
  void handleRequest(::AsyncWebServerRequest *request) {
    float flow_lmin = 0.0f, total_m3 = 0.0f;
    if (this->flow_  && std::isfinite(this->flow_->state))  flow_lmin = this->flow_->state;
    if (this->total_ && std::isfinite(this->total_->state)) total_m3  = this->total_->state;
    long total_liters = lroundf(total_m3 * 1000.0f);
    char body[128];
    snprintf(body, sizeof(body), "{\"waterflow\":\"%.0f\",\"waterquantity\":\"%ld\"}", flow_lmin, total_liters);
    request->send(200, "application/json", body);
  }
  bool isRequestHandlerTrivial() { return true; }

 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};

  void attach_or_retry_() {
    if (web_server_base::global_web_server_base != nullptr) {
      web_server_base::global_web_server_base->add_handler(this);
      ESP_LOGI("water_json", "Registered /json.html handler");
    } else {
      ESP_LOGW("water_json", "Web server not ready, will retry...");
      this->set_interval(1000, [this]() {
        if (web_server_base::global_web_server_base != nullptr) {
          web_server_base::global_web_server_base->add_handler(this);
          ESP_LOGI("water_json", "Registered /json.html handler (after retry)");
          this->cancel_interval();
        } else {
          ESP_LOGW("water_json", "Web server still not ready, retrying...");
        }
      });
    }
  }
};
#else
class WaterJsonHandler : public Component {
 public:
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) { this->flow_ = flow_sensor; this->total_ = total_sensor; }
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }
  void setup() override { ESP_LOGW("water_json", "Custom /json.html endpoint not available on non‑Arduino backend."); }
 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};
};
#endif

}  // namespace water_json_handler
}  // namespace esphome
