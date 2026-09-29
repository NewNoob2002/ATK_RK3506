# P4 页面与按键迁移

2026-09-24 完成 P4 离线切片；2026-09-28 在 RK3506J 测试板完成按键与部分页面实屏试运行；2026-09-28 完成 Debug 监视器实屏复核，P4 验收关闭。源代码来源及改动边界见 [SOURCE.md](../application/App/SOURCE.md)。

## 已接入

- 原 `PageManager` 的页面栈、生命周期和切换逻辑，原 `DialplateView`、资源、状态栏布局及数字组件。状态栏现在由 `StatusBarState` 更新；未接业务数据时显示 `DEMO`，隐藏未知的卫星数、时间和电量数值。
- 九个页面的 View 布局已迁入：主界面、工作设置、记录配置、系统信息、关机、星图、启动、系统加载（原 `HardwareCheck`）、保存配置。中间页现名 `Pages/SystemLoading`：没有真实硬件检查结果时不称“硬件检查”。主界面五个按钮沿用旧页面名路由；无按键时可静态选页。未知数值显示为 `N/A` 或 `-`；业务动作尚未接入。
- 测试板原外接按键位于 P1_B3，曾与 UART2 TX/sysfs 冲突；当前输入代码改从 Linux evdev 读取 `KEY_VOLUMEUP` / `KEY_VOLUMEDOWN` / `KEY_MENU` / `KEY_ESC` 的按下事件，分别用于下一焦点、上一焦点、确认、返回。此测试板已确认 `adc-keys` 对应 `/dev/input/event0`；旧 GPIO1_B4 方案不再是应用输入入口。
- `p4_screen` 复用 P3 的 SPI mode 3 屏驱动和 LVGL display port。`--display-only --page Pages/NAME` 跳过按键设备并选择单页；省略 `--page` 从 Startup 待机页开始。SIGINT/SIGTERM 退出，释放 evdev、GPIO、SPI 和 LVGL 资源。状态栏业务数据未接入，屏上显示的是演示 UI。

主机默认配置 CTest 6/6 通过（包含 evdev 模拟事件与页面按键导航）；ARM32 Buildroot 工具链编译 `p4_screen`、`button_probe` 和测试程序，两个按键测试在 QEMU 通过。`out/p4/screens.png` 是九页本机软件渲染预览，不能代替实屏观察。CI 已配置独立检查默认应用和 benchmark 构建；当前提交的远端运行结果尚待观察。

## 2026-09-28 实板按键试运行

通过 `/dev/ttyACM0`（1500000，未触发复位）确认 `ATK-DLRK3506J Board`、Linux 6.1.118；板端 `/tmp/button_probe`、`/tmp/p4_screen` 与本机 ARM 构建 SHA256 一致。`/proc/bus/input/devices` 中 `adc-keys` 为 `/dev/input/event0`，已有 evtest 记录四键按下/松开。限时运行 `/tmp/button_probe /dev/input/event0`，操作者各按一次，依次采到 `V+ next-focus`、`V- previous-focus`、`MENU confirm`、`ESC back`，采集结束后进程已停止。

以 SPI mode 3、32 MHz 请求档限时运行 `/tmp/p4_screen /dev/spidev0.0 /dev/gpiochip1 32000000 /dev/input/event0`；日志记录焦点动作、MENU 进入 SystemInfos/RecordConfig/WorkSettings、ESC 返回 Dialplate。第二次专项运行记录 `V+ → MENU → Pages/Shutdown → ESC → Pages/Dialplate`，操作者确认关机页面实际显示且可返回。测试程序已退出；退出后屏幕保留最后画面，不代表应用仍响应按键。当前页面加载动画在 `P4App` 中配置为 `LOAD_ANIM_NONE`，所以进入/退出无动画是预期行为。英俄切换、状态栏生命周期、实际业务操作及长时间运行仍未验收。

随后用相同的板端产物以 `--display-only --page Pages/NAME` 顺序显示九页，每页限时 12 秒，均正常退出。操作者发现 RecordConfig 时钟图标偏离右侧选项栏、SaveConfig 品牌标志侵入状态栏；其他页面未报告异常。已在本机将时钟图标对齐到右侧选项栏、把 SaveConfig 标志和文案下移到 26 px 状态栏以下；主机 CTest 6/6 通过，SDL 1:1 本机预览经操作者确认这两处位置符合预期。修正版已通过串口 1500000 部署到 `/tmp/p4_screen` 和 `/tmp/button_probe`，SHA256 与 ARM 构建一致；九页修正版均在 32 MHz 请求档限时运行并以 0 退出。当前仍缺少操作者对 RecordConfig、SaveConfig 两页修正版的逐页目视确认，不能把布局对照记为最终通过。

## 本机 SDL 页面预览

安装 SDL2 开发库后，在项目根目录运行：

```sh
cmake -S application -B out/sdl -DCMAKE_BUILD_TYPE=Debug -DRK3506_SDL_PREVIEW=ON
cmake --build out/sdl --target p4_sdl
out/sdl/p4_sdl
```

窗口为与原 Simulator 相同的 294×126、1:1 像素尺寸，复用同一 `P4App` 与 LVGL 配置，不触碰板端设备。启动显示 Startup：鼠标左键或 Enter **按住约 2 秒**，经过 SystemLoading（约 2.2 秒）进入主表盘；主表盘的关机按钮进入 Shutdown，按住关机按钮约 2 秒进入 SaveConfig（进度演示约 8.5 秒），然后回到 Startup。鼠标左键点击可操作按钮、中键确认焦点、右键返回、滚轮移动焦点；键盘方向键移动焦点、短按 Enter 确认、Esc 返回、Q 退出。工作设置/记录配置的方向按钮可预览选项，返回按钮可回主界面；系统信息条目及星图可聚焦并点击返回，关机页还可切换语言。**整个开关机流程只演示页面**，不执行真实硬件检查、配置保存、系统断电或重启；未接入业务后端的模式、录制、Wi-Fi 按钮仍禁用。页面只通过焦点按钮导航，不另设翻页快捷键。页面管理器已沿用 [X-TRACK](https://github.com/FASTSHIFT/X-TRACK/tree/main/Software/X-Track) 同系生命周期/路由机制，本轮只补齐事件处理与页面切换，不再引入另一套管理器。SDL 窗口不能替代真实屏幕遮挡和时序验收。

## 后续实屏检查

**可以先做 P4 静态实屏试运行**，逐页检查布局和外壳遮挡；无需等待按键设备确认。试运行前保持 P3 已确认的电平/共地和 SPI0 独占条件；静态试运行使用 `--display-only`。

后续开始 P4 实屏检查时，使用与 P3 benchmark 相同的设备节点、SPI mode 3 和已目视验证的 32 MHz 请求档，限时运行：

```text
timeout -s TERM 90 ./p4_screen --display-only --page Pages/StarMap /dev/spidev0.0 /dev/gpiochip1 32000000
```

这会操作 RST/DC、重新初始化并写屏。验证按键前，先在目标系统通过 `/proc/bus/input/devices` 与受控按键事件确认正确的 `/dev/input/eventN` 及四个键码，不要假设 `event0` 或沿用旧 `/dev/gpiochip1` 输入方式。限时运行 `timeout -s TERM 30 ./button_probe /dev/input/eventN`，逐个检查 V+/V-/ESC 只在按下触发、MENU 分别在按下与松开产生事件、重复事件不触发；再用 `timeout -s TERM 90 ./p4_screen /dev/spidev0.0 /dev/gpiochip1 32000000 /dev/input/eventN` 检查焦点、确认与返回。若板上没有相应 evdev 键码，需要先核实接线及内核输入配置，不能仅靠应用编译结果判定按键已接通。

P3 benchmark 在此请求档正常显示并完成整轮测试，但其帧率不能代替 P4 页面实测。P4 仍需核对画面、刷新时长和输入响应；47 MHz 请求档曾无法点亮屏幕，不能作为提速方案。物理 CS/DC/SCLK 波形尚未测量。

P4 验收已完成：修正版已重新部署；九页均以 32 MHz 请求档运行退出；操作者已现场验证 V+/V-/MENU/ESC 导航、页面返回及 Debug 监视器显示的 FPS、CPU、内存指标均正常。运行日志记录了启动、加载、关机、保存配置和多页返回路径；另有一次 LVGL `active screen was deleted` 警告，需在 P6 稳定性阶段复现并判断是否为退出清理时序。实际业务数据与设置回执属于 P5；量产 PCB 的按键/电源方案仍待定型。
