# P4 页面与按键迁移

2026-09-24：当前是**离线验证通过的 P4 切片**，尚未在 RK3506J 实屏运行。源代码来源及改动边界见 [SOURCE.md](../application/App/SOURCE.md)。

## 已接入

- 原 `PageManager` 的页面栈、生命周期和切换逻辑，原 `DialplateView`、资源、状态栏布局及数字组件。状态栏现在由 `StatusBarState` 更新；未接业务数据时显示 `DEMO`，隐藏未知的卫星数、时间和电量数值。
- 原九个页面的 View 布局已迁入：主界面、工作设置、记录配置、系统信息、关机、星图、启动、硬件检查、保存配置。主界面五个按钮沿用旧页面名路由；无按键时可静态选页。未知数值显示为 `N/A` 或 `-`；业务动作尚未接入。
- 测试板按键原接 P1_B3 至 GND、无外部上拉；B3 与运行中 UART2 TX/sysfs 冲突。按键入口预留 GPIO1_B4（JP1-28，bank offset 12）及内部上拉；改接和交互验证后置。
- `p4_screen` 复用 P3 的 SPI mode 3 屏驱动和 LVGL display port。`--display-only --page Pages/NAME` 跳过按键申请并选择单页；省略 `--page` 显示主界面。SIGINT/SIGTERM 退出，释放 GPIO、SPI 和 LVGL 资源。状态栏业务数据未接入，屏上显示的是演示 UI。

主机默认配置 CTest 5/5 通过；ARM32 Buildroot 工具链编译 `p4_screen` 与 `p4_navigation_preview`，后者在 QEMU 通过。`out/p4/screens.png` 是九页本机软件渲染预览，不能代替实屏观察。CI 已配置独立检查默认应用和 benchmark 构建；当前提交的远端运行结果尚待观察。

## 什么时候上实屏

**现在可以先做 P4 静态实屏试运行**，逐页检查布局和外壳遮挡；无需等待按键改接。试运行前保持 P3 已确认的电平/共地和 SPI0 独占条件。当前 B3 接线不能用于程序的按键入口，静态试运行使用 `--display-only`。

后续开始 P4 实屏检查时，使用与 P3 benchmark 相同的设备节点、SPI mode 3 和已目视验证的 32 MHz 请求档，限时运行：

```text
timeout -s TERM 90 ./p4_screen --display-only --page Pages/StarMap /dev/spidev0.0 /dev/gpiochip1 32000000
```

这会操作 RST/DC、重新初始化并写屏。若要验证按键，先将接地按键改接至 JP1-28（GPIO1_B4），在运行中的系统确认 `/dev/gpiochip1` 是 GPIO1、offset 12 的 pinmux 是 GPIO 且无 consumer 占用；再单独限时运行 `timeout -s TERM 30 ./button_probe /dev/gpiochip1`，确认松开为 HIGH、按下为 LOW。之后去掉 `--display-only` 运行 `p4_screen`，检查焦点与返回。GPIO 申请失败时不要强行更改正在使用的引脚。

P3 benchmark 在此请求档正常显示并完成整轮测试，但其帧率不能代替 P4 页面实测。P4 仍需核对画面、刷新时长和输入响应；47 MHz 请求档曾无法点亮屏幕，不能作为提速方案。物理 CS/DC/SCLK 波形尚未测量。

P4 完整验收还需要英俄切换、状态栏/页面更新生命周期，并与旧 Simulator 和实屏画面核对。实际业务数据与设置回执属于 P5；按键改接与交互验证后置。屏外壳遮住部分边缘，实屏检查应记录可见区域及被遮挡项。
