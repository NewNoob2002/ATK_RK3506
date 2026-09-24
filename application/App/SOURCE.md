# P4 原应用源码来源

- 来源：`/home/gtc/Desktop/workspace/HC32_PROJ/HC32F460xE_Arduino/Simulator/App`，源仓库提交 `37b069a0e76061dbac07e9345e653d1796945bac`。所复制文件在该提交上无本地修改。
- 已迁入 `Pages/Dialplate/DialplateView.*`、`Resource/`、直接使用的 `Utils/` 组件，以及原 `Utils/PageManager/` 全部源码与 `numberFlow_clock`；保留原许可说明。PageManager 仅调整 LVGL 头文件引用并删除一个不可达空语句。
- `Pages/StatusBar/` 沿用旧界面的资源和布局，改由 `StatusBarState` 显式输入，移除对 HC32 `systemInfo`、`DataProc` 和 `Account` 的依赖；原充电动画、业务订阅与语言切换尚未迁入。
- `P4App` 复用原 PageManager 的页面栈、焦点组和主界面按钮路由。原九个页面的 View 布局均已迁入；启动、硬件检查与保存配置也可静态选页。电源流程、硬件检查结果、设置操作与业务状态尚未接入。
- 按键入口预留 GPIO1_B4 内部上拉；GPIO1_B3 与运行中 UART2 TX/sysfs 冲突。按键改接与交互验证后置；原 `DataProc`、HAL 和业务状态尚未迁入。
- `p4_navigation_preview Pages/Startup output.ppm` 可输出单页预览；`p4_screen --display-only --page Pages/Startup` 可在无按键时选页。ARM 程序已构建，但 P4 UI 未实屏运行；软件测试不代表屏幕刷新性能通过。
