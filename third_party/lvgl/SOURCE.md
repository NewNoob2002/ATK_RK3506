# LVGL 源码基线

- 版本：8.3.11，由 lvgl.h 中版本宏确认。
- 来源：/home/gtc/Desktop/workspace/HC32_PROJ/HC32F460xE_Arduino/Libraries/lvgl。
- 来源仓库提交：28e89c1406e2c26ded7b402fa37aadf2c6d9155e。
- 提取日期：2026-09-18；仅复制 lvgl.h 和 src/，377 个文件逐字节核对一致。
- 初始基线未复制 Arduino port、示例与模拟器 SDL port；没有修改 LVGL 内核。后续为显示性能试验，单独复制了同版本的 benchmark demo。
- application/config/lv_conf.h 初始为原配置：RGB565、SWAP=1、128 KiB 内存池、外部 lv_tick_inc。现在仅在 RK3506_LVGL_DEMO 构建中启用 benchmark 所需组件与 POSIX 单调时钟，并允许调整刷新周期；常规构建仍保持原值。
- 初始配置 SHA256：01ca6ad32c83cad400b03e97a5e433371c40469d155a556d9b62a797a4be7de0；该值不代表后续条件编译版本的文件哈希。
- 源码聚合 SHA256：3dd24cf8c5d705132a036ccae0dbdf33b6d01de74ecedc976531c93d6c5e743e。算法：按相对路径排序，仅对 lvgl.h 和 src/ 文件，顺次输入 UTF-8 路径、NUL 字节、原始文件内容。

保留源文件中的版权/许可注释。来源目录没有独立 LICENSE 文件，本次未臆造或从其他版本替换；对外分发前需补齐与此版本匹配的许可文本。

`demos/` 的 12 个文件来自 `/home/gtc/Desktop/workspace/HC32_PROJ/HC32F460xE_Arduino/Simulator/lvgl/demos/`，来源提交 `37b069a0e76061dbac07e9345e653d1796945bac`。复制了 `lv_demos.h`、benchmark 的 C/头文件及 9 个 C 资源；仅为通过当前严格编译警告，对 benchmark 的场景索引做有符号边界检查，并标记未使用的回调参数。
