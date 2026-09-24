# RK3506J RM690A0 / LVGL 应用迁移

目标：移除原 HC32，由 ATK RK3506J Linux 平台直接驱动现有 RM690A0 彩色屏，并迁移 HC32 工程的 294×126、RGB565、LVGL 8.3.11 应用。P2 离线构建基线已完成；P3 屏驱动、Linux IO、LVGL 显示 port 与测试图已实现。2026-09-24 在测试板上以 SPI mode 3、500 kHz 点亮 AMOLED，颜色和方向经目视确认，再次启动成功。外壳遮挡部分边缘；物理波形、最高速率和 SGM3849 输出轨尚未实测。原业务应用尚未迁移。

当前测试板为用户确认的 RK3506J 512MB+8GB；P1 显示接口已定型，沿用现有屏供电，不增加背光亮度控制。实体按键与整机电源移植延后到量产 PCB 阶段。P2 已使用 J/eMMC 基线完成构建，具体介质与运行配置在板端核实。

- [架构与实施计划](docs/PROJECT_PLAN.md)：模块职责、硬件依据、阶段交付与验收。
- [HC32 迁移摘要](docs/HC32_MIGRATION.md)：实际初始化、旋转/字节序、原应用结构、业务与电源接口的迁移边界。
- [P1 硬件定型记录](board/rk3506j/HARDWARE.md)：用户实接线、候选设备树冲突核查、电气与系统职责待定项。
- [P2 构建基线与复现](docs/P2_BASELINE.md)：SDK 构建结果、Simulator 修复及应用 CMake。
- [P3 屏驱动实施与验收](docs/P3_DISPLAY.md)：协议、SPI/GPIO、测试图、实屏结果与剩余验收项。
- [Docker 编译环境](docs/BUILD_ENVIRONMENT.md)：镜像构建、启动、SDK 编译入口与验证边界。
- `application/`：已建立 CMake、LVGL 基线、RM690A0/Linux IO 和显示测试入口，原业务应用在 P4 迁移。
- `tests/`：协议、显示模型和 Linux IO 的主机测试。
- `refer/01_lvgl_button/`：用户提供的已实屏运行参考程序源码；就地构建产物不入版本库。
- `third_party/lvgl/`：固定原 MCU 的 LVGL 8.3.11，保留来源记录。
- `sdk/`：厂商 SDK 工作区，已由现有 `.gitignore` 排除；SDK 改动以补丁保存在主项目。
- `docker/`：Ubuntu 20.04 编译环境与最小验证脚本。
- `.github/workflows/host-tests.yml`：无需 SDK 或硬件的主机 CMake/CTest CI；本地复现见下方命令。

在项目根目录运行主机检查：

```sh
cmake -S application -B out/ci -DCMAKE_BUILD_TYPE=Debug
cmake --build out/ci --parallel 2
ctest --test-dir out/ci --output-on-failure
```

原始资料保留在 `docs/`；实际 SDK 位于 `sdk/atk_dlrk3506_linux6.1_release_v1.3.1_20260326/`。P3 节点保存为本地 Git 标签 `p3-display-bringup-2026-09-24`；标签不代表剩余物理验收项已完成。
