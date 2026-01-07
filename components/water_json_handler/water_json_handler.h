
// components/water_json_page/water_json_page.h
#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/web_server/web_server.h"

namespace esphome {
namespace water_json_page {

class WaterJsonPage : public Component {
 public:
  void set_flow_sensor(sensor::Sensor *s) { flow_ = s; }
  void set_total_sensor(sensor::Sensor *s) { total_ = s; }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void setup() override {
#ifdef USE_WEBSERVER
    using web_server::WebServer;
    auto *ws = WebServer::global_web_server;
    if (ws == nullptr) return;
    ws->get_web_server_base()->add_handler(
      new web_server_base::StaticRequestHandler(
        "/json.html",
        [this](AsyncWeb  char buf[128];
          float flow  = this->flow_  ? this->flow_->state  : NAN;
          float total = this->total_ ? this->total_->state : NAN;
          // Adjust formatting as you like
          int n = snprintf(buf, sizeof(buf),
                           "{\"waterflow\":\"%.1f\",\"waterquantity\":\"%.3f\"}",
                           isfinite(flow) ? flow : 0.0f,
                           isfinite(total) ? total : 0.0f);
          auto *resp = request->beginResponse(200, "application/json",
                                              String(buf).substring(0, n));
          request->send(resp);
        }
      )
    );
#endif
  }

 protected:
  sensor::Sensor *flow_{nullptr};
  sensor::Sensor *total_{nullptr};
};

}  // namespace water_json_page
}  // namespace esphome
