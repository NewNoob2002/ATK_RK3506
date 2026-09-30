# P4 原应用源码来源

- 来源：`/home/gtc/Desktop/workspace/HC32_PROJ/HC32F460xE_Arduino/Simulator/App`，源仓库提交 `37b069a0e76061dbac07e9345e653d1796945bac`。所复制文件在该提交上无本地修改。
- 已迁入 `Pages/Dialplate/DialplateView.*`、`Resource/`、直接使用的 `Utils/` 组件，以及原 `Utils/PageManager/` 全部源码和数字时钟组件（现类名 `NumberFlowClock`）；保留原许可说明。初次移植调整了 LVGL 头文件引用并删除不可达空语句；后续已修改 PageManager 的输入/路由协作、卸载清理与参数边界，不再是仅头文件适配。提交 `c1481b5` 加固公共工具边界，`7b49532` 统一命名和格式，并同步更新调用方与测试。
- `Pages/StatusBar/` 沿用旧界面的资源和布局，改由 `StatusBarState` 显式输入，移除对 HC32 `systemInfo`、`DataProc` 和 `Account` 的依赖；原充电动画和业务订阅尚未迁入；页面语言切换已由 `i18n::set_language()` 与 `PageManager::notify_language_changed()` 接入，不能将状态栏旧订阅机制缺失描述为整个应用没有语言切换。
- `P4App` 复用原 PageManager 的页面栈、焦点组和主界面按钮路由。九个页面的 View 布局均已迁入；原 `HardwareCheck` 中间页已重命名为 `SystemLoading`，避免暗示尚未接入的硬件检查结果。启动、加载与保存配置目前是演示流程；电源流程、硬件检查结果、设置操作与业务状态尚未接入。
- 测试板按键现通过 Linux evdev `KEY_VOLUMEUP` / `KEY_VOLUMEDOWN` / `KEY_MENU` / `KEY_ESC` 输入；已在板端确认 `adc-keys` 的 `/dev/input/event0` 并完成四键试运行。GPIO1_B4 不再是应用输入入口。原 `DataProc`、HAL 和业务状态尚未迁入。
- 当前使用 `DemoStatus` 生成确定性的占位状态快照，由 `p4_screen` 主循环按 200 ms 节奏调用 `P4App::update_status()`；SDL 预览当前不调用此生产者。真实数据可替换生产者而不改变状态栏视图契约。LVGL 的 `lv_tick_inc()`、`lv_timer_handler()`、页面管理、输入和控件更新保持在同一主线程；当前不需要为 tick 单独创建子线程。未来异步生产者只能发布快照/事件，由 LVGL 主循环消费。
- 当前页面切换配置为 `LOAD_ANIM_MOVE_LEFT`、250 ms、`lv_anim_path_ease_out`，提交 `4c6a8d5` 已恢复动画。模式、录制和 Wi-Fi 业务按钮保持禁用，开关机长按仅控制演示流程。
- `p4_navigation_preview Pages/Startup output.ppm` 可输出单页预览；板端选页示例为 `p4_screen --display-only --page Pages/Startup /dev/spidev0.0 /dev/gpiochip1 32000000`，设备与速率须按目标条件核实并获得操作授权。`p4_status_test` 覆盖占位状态的时间、范围和开关变化。当前主机默认测试 7/7、独立 benchmark 测试 1/1 通过；既有 ARM 产物已在测试板运行，但此次未重新构建或部署 `7b49532`。软件测试不代表屏幕刷新性能或真实业务通过；验收范围与后续项见 [P4 记录](../../docs/P4_APPLICATION.md)。
