# RK3506J RM690A0 / LVGL 应用迁移

目标：移除原 HC32，由 ATK RK3506J Linux 平台直接驱动现有 RM690A0 彩色屏，并迁移 HC32 工程的 294×126、RGB565、LVGL 8.3.11 应用。P3 已在测试板用 SPI mode 3 点亮测试图，颜色和方向经目视确认；LVGL benchmark 在 32 MHz 请求、8 ms 刷新配置下整轮平均 57.3 FPS，详见 [P3 实屏性能记录](docs/P3_DISPLAY.md#lvgl-benchmark-实屏性能2026-09-24)。P4 已完成原 PageManager、状态栏及九个页面的 View 布局迁移，主机与 ARM 验证通过；测试板已完成九页限时运行、evdev 四键导航、页面返回及 Debug FPS/CPU/内存监视器复核。业务数据与操作进入 P5。外壳遮挡部分边缘；物理波形、最高速率和 SGM3849 输出轨尚未实测。

当前测试板为用户确认的 RK3506J 512MB+8GB；P1 显示接口已定型，沿用现有屏供电，不增加背光亮度控制。测试板原按键接线 P1_B3 曾与 UART2 TX/sysfs 冲突；当前测试板 `adc-keys` 通过 `/dev/input/event0` 提供 V+、V-、MENU、ESC 四键，已板端验证焦点与页面导航。量产按键和整机电源设计仍在量产 PCB 阶段定型。P2 已使用 J/eMMC 基线完成构建，具体介质与运行配置在板端核实。

- [架构与实施计划](docs/PROJECT_PLAN.md)：模块职责、硬件依据、阶段交付与验收。
- [HC32 迁移摘要](docs/HC32_MIGRATION.md)：实际初始化、旋转/字节序、原应用结构、业务与电源接口的迁移边界。
- [P1 硬件定型记录](board/rk3506j/HARDWARE.md)：用户实接线、候选设备树冲突核查、电气与系统职责待定项。
- [P2 构建基线与复现](docs/P2_BASELINE.md)：SDK 构建结果、Simulator 修复及应用 CMake。
- [P3 屏驱动实施与验收](docs/P3_DISPLAY.md)：协议、SPI/GPIO、测试图、实屏结果与剩余验收项。
- [P4 应用迁移与实屏试运行](docs/P4_APPLICATION.md)：页面路由、状态栏、按键行为及试运行条件。
- [Docker 编译环境](docs/BUILD_ENVIRONMENT.md)：镜像构建、启动、SDK 编译入口与验证边界。
- `application/`：已有 LVGL 基线、RM690A0/Linux IO、显示测试入口及 [P4 主界面来源记录](application/App/SOURCE.md)；原业务应用正在迁移。
- `tests/`：协议、显示模型和 Linux IO 的主机测试。
- `refer/01_lvgl_button/`：用户提供的已实屏运行参考程序源码；就地构建产物不入版本库。
- `third_party/lvgl/`：固定原 MCU 的 LVGL 8.3.11，保留来源记录。
- `sdk/`：厂商 SDK 工作区，已由现有 `.gitignore` 排除；SDK 改动以补丁保存在主项目。
- `docker/`：Ubuntu 20.04 编译环境与最小验证脚本。
- `.github/workflows/host-tests.yml`：默认应用与 benchmark 分别构建、测试；[P3 检查点运行结果](https://github.com/NewNoob2002/ATK_RK3506/actions/runs/35965777501)仅覆盖旧版 3 项测试。

在项目根目录运行主机检查：

```sh
cmake -S application -B out/ci -DCMAKE_BUILD_TYPE=Debug
cmake --build out/ci --parallel 2
ctest --test-dir out/ci --output-on-failure
```

benchmark 使用独立构建配置：`cmake -S application -B out/ci-demo -DCMAKE_BUILD_TYPE=Debug -DRK3506_LVGL_DEMO=ON`，随后构建并运行该目录的 CTest。

离线单页预览可运行 `out/ci/p4_navigation_preview Pages/StarMap out/ci/star-map.ppm`；不带页面名时检查九页导航。预览不包含真实业务状态或板端刷新性能。九页拼图见 `out/p4/screens.png`（本机生成文件）。

原始资料保留在 `docs/`；实际 SDK 位于 `sdk/atk_dlrk3506_linux6.1_release_v1.3.1_20260326/`。P3 代码检查点为提交 `20f7f1c`，本地标签为 `p3-display-bringup-2026-09-24`；标签不代表剩余物理验收项已完成。
