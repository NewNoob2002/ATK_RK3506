#pragma once
#include <array>
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

namespace page {
class SystemDashView {
  public:
    void create(lv_obj_t* root);
    void destroy() {}
    void apply_language() const;
    void show_power(bool visible);
    bool power_open() const {
        return power_open_;
    }
    std::array<lv_obj_t*, 3> controls() const;
    std::array<lv_obj_t*, 6> all_controls() const;
    struct {
        lv_obj_t *power{}, *settings{}, *back{}, *overlay{}, *shutdown{}, *reboot{}, *cancel{};
    } ui;

  private:
    struct Caption {
        lv_obj_t* label;
        i18n::TextId id;
    };
    std::array<Caption, 9> captions_{};
    unsigned caption_count_ = 0;
    bool power_open_ = false;
    lv_obj_t* caption(lv_obj_t* parent, i18n::TextId id, int x, int y, int width, bool small = false);
};
} // namespace page
