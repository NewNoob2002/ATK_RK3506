# P4 原应用源码来源

- 来源：`/home/gtc/Desktop/workspace/HC32_PROJ/HC32F460xE_Arduino/Simulator/App`，源仓库提交 `37b069a0e76061dbac07e9345e653d1796945bac`。所复制文件在该提交上无本地修改。
- 已迁入 `Pages/Dialplate/DialplateView.*`、`Resource/`、直接使用的 `Utils/` 组件，以及原 `Utils/PageManager/` 全部源码与 `numberFlow_clock`；保留原许可说明。PageManager 仅调整 LVGL 头文件引用并删除一个不可达空语句。
- `Pages/StatusBar/` 沿用旧界面的资源和布局，改由 `StatusBarState` 显式输入，移除对 HC32 `systemInfo`、`DataProc` 和 `Account` 的依赖；原充电动画、业务订阅与语言切换尚未迁入。
- `P4App` 复用原 PageManager 的页面栈、焦点组和主界面按钮路由。九个页面的 View 布局均已迁入；原 `HardwareCheck` 中间页已重命名为 `SystemLoading`，避免暗示尚未接入的硬件检查结果。启动、加载与保存配置目前是演示流程；电源流程、硬件检查结果、设置操作与业务状态尚未接入。
- 测试板按键现通过 Linux evdev `KEY_VOLUMEUP` / `KEY_VOLUMEDOWN` / `KEY_MENU` / `KEY_ESC` 输入；已在板端确认 `adc-keys` 的 `/dev/input/event0` 并完成四键试运行。GPIO1_B4 不再是应用输入入口。原 `DataProc`、HAL 和业务状态尚未迁入。
- 当前使用 `DemoStatus` 生成确定性的占位状态快照，按 200 ms 节奏由主循环调用 `P4App::UpdateStatus()`；真实数据可替换生产者而不改变状态栏视图契约。LVGL 的 `lv_tick_inc()`、`lv_timer_handler()`、页面管理、输入和控件更新保持在同一主线程；当前不需要为 tick 单独创建子线程。未来异步生产者只能发布快照/事件，由 LVGL 主循环消费。
- `p4_navigation_preview Pages/Startup output.ppm` 可输出单页预览；`p4_screen --display-only --page Pages/Startup` 可在无按键时选页。`p4_status_test` 覆盖占位状态的时间、范围和开关变化。ARM 程序已在测试板运行；软件测试不代表屏幕刷新性能通过。
