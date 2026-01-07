
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

class WaterJsonHandler : public Component {
 public:
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) {
    this->flow_  = flow_sensor;   // l/min
    this->total_ = total_sensor;  // m³
  }

  // Run after the web_server initializes
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }

  void setup() override {
    this->attach_or_retry_();
  }

 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};

  void attach_or_retry_() {
    static constexpr const char* kRetryName = "json_register_retry";

    auto *base = web_server_base::global_web_server_base;
    if (base != nullptr) {
      auto *server = base->get_server();   // underlying AsyncWebServer*
      if (server != nullptr) {
        // Primary endpoint
        server->on("/json.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
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
        });

        // Optional alias: /json
        server->on("/json", HTTP_GET, [this](AsyncWebServerRequest *request) {
          float flow_lmin = (this->flow_  && std::isfinite(this->flow_->state))  ? this->flow_->state  : 0.0f;
          float total_m3  = (this->total_ && std::isfinite(this->total_->state)) ? this->total_->state : 0.0f;
          long total_liters = lroundf(total_m3 * 1000.0f);
          char body[128];
          snprintf(body, sizeof(body),
                   "{\"waterflow\":\"%.0f\",\"waterquantity\":\"%ld\"}",
                   flow_lmin, total_liters);
          request->send(200, "application/json", body);
        });

        ESP_LOGI("water_json", "Registered /json.html and /json handlers");
        return;
      }
    }

    // Not ready yet → retry every 1000 ms
    ESP_LOGW("water_json", "Web server not ready; will retry...");
    this->set_interval(kRetryName, 1000, [this]() {
      auto *base = web_server_base::global_web_server_base;
      if (base != nullptr && base->get_server() != nullptr) {
        auto *server = base->get_server();
        server->on("/json.html", HTTP_GET, [this](AsyncWebServerRequest *request) {
          float flow_lmin = (this->flow_  && std::isfinite(this->flow_->state))  ? this->flow_->state  : 0.0f;
          float total_m3  = (this->total_ && std::isfinite(this->total_->state)) ? this->total_->state : 0.0f;
          long total_liters = lroundf(total_m3 * 1000.0f);
          char body[128];
          snprintf(body, sizeof(body),
                   "{\"waterflow\":\"%.0f\",\"waterquantity\":\"%ld\"}",
                   flow_lmin, total_liters);
          request->send(200, "application/json", body);
        });
        server->on("/json", HTTP_GET, [this](AsyncWebServerRequest *request) {
          float flow_lmin = (this->flow_  && std::isfinite(this->flow_->state))  ? this->flow_->state  : 0.0f;
          float total_m3  = (this->total_ && std::isfinite(this->total_->state)) ? this->total_->state : 0.0f;
          long total_liters = lroundf(total_m3 * 1000.0f);
          char body[128];
          snprintf(body, sizeof(body),
                   "{\"waterflow\":\"%.0f\",\"waterquantity\":\"%ld\"}",
                   flow_lmin, total_liters);
          request->send(200, "application/json", body);
        });
        ESP_LOGI("water_json", "Registered /json.html and /json handlers (after retry)");
        this->cancel_interval(kRetryName);
      } else {
        ESP_LOGW("water_json", "Web server still not ready; retrying...");
      }
    });
  }

};

#else  // Non‑Arduino backend

class WaterJsonHandler : public Component {
 public:
  void set_sources(sensor::Sensor *flow_sensor, sensor::Sensor *total_sensor) {
    this->flow_  = flow_sensor;
    this->total_ = total_sensor;
  }
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }
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
