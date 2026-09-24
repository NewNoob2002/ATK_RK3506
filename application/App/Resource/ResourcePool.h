#pragma once
#include "lvgl.h"
#include "symbol/symbol_unicode.h"

namespace ResourcePool {

/** 在 lv_init() 后调用一次，注册静态编译的字体和图片资源。 */
void Init();

/** 返回资源池持有的字体；名称缺失时返回 LV_FONT_DEFAULT。 */
lv_font_t* GetFont(const char* name);

/** 返回资源池持有的图片；名称缺失时返回 nullptr。 */
const void* GetImage(const char* name);

}
