#!/usr/bin/env python3
"""重绘共享 UI 原生尺寸图标；需要项目环境已有的 ImageMagick，不在固件运行时缩放。"""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1] / "application/App/Resource/img"
# 原创 16x16 矢量路径；info 的三横线为现有 Device Info 图标的视觉基准。
PATHS = {
    "info": '<path d="M2 3h12M2 8h12M2 13h12"/>',
    "work": '<path d="M2 3h12M2 8h12M2 13h12" stroke-width="1.6"/><path d="M5 2v2M11 7v2M5 12v2" stroke-width="3"/>',
    "record": '<path d="M3 2h7l3 3v9H3zM6 2v4h4V2M6 10h4"/>',
    "settings": '<path d="M6 2V1h4v1l2 1 1-1 2 3-1 1v4l1 1-2 3-1-1-2 1v1H6v-1l-2-1-1 1-2-3 1-1V6L1 5l2-3 1 1z"/><circle cx="8" cy="8" r="2"/>',
    "base": '<path d="M8 5v9M4 14h8M8 8l-3 6M8 8l3 6M5 3a4 4 0 0 0 0 4M11 3a4 4 0 0 1 0 4M2 1a7 7 0 0 0 0 8M14 1a7 7 0 0 1 0 8"/><circle cx="8" cy="4" r="1" fill="white" stroke="none"/>',
    "rover": '<path d="M8 2l6 12-6-3-6 3zM8 6v5"/>',
    "network": '<circle cx="8" cy="8" r="6"/><path d="M2 8h12M8 2c-4 3-4 9 0 12M8 2c4 3 4 9 0 12"/>',
    "protocol": '<path d="M1 8h2l2-5 4 10 2-5h4"/>',
    "channel": '<path d="M3 2v12M8 2v12M13 2v12M1 5h4M6 11h4M11 6h4"/>',
    "clock": '<circle cx="8" cy="8" r="6"/><path d="M8 4v4l3 2"/>',
    "apply": '<path d="M2 8l4 4L14 4"/>',
    "back": '<path d="M7 3L2 8l5 5M2 8h12"/>',
    "play": '<path d="M4 2l9 6-9 6z" fill="white" stroke="none"/>',
    "stop": '<rect x="3" y="3" width="10" height="10" rx="1" fill="white" stroke="none"/>',
    "power": '<path d="M8 1v6M4 3a6 6 0 1 0 8 0"/>',
    "wifi": '<path d="M2 5a10 10 0 0 1 12 0M4 8a6 6 0 0 1 8 0M6 11a3 3 0 0 1 4 0"/><circle cx="8" cy="14" r="1" fill="white" stroke="none"/>',
}


def render(path: str, size: int) -> bytes:
    svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{size * 4}" height="{size * 4}" viewBox="0 0 16 16">'
           '<g fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">'
           f'{path}</g></svg>')
    pixels = subprocess.run(["magick", "-background", "none", "svg:-", "-resize", f"{size}x{size}!",
                             "-alpha", "extract", "-auto-level", "-depth", "8", "gray:-"], input=svg.encode(),
                            stdout=subprocess.PIPE, check=True).stdout
    assert len(pixels) == size * size and max(pixels) >= 250
    assert 0.1 < sum(pixels) / (255 * size * size) < 0.85
    return pixels


def main() -> None:
    images = {}
    for name, path in PATHS.items():
        sizes = (32,) if name in {"base", "rover"} else (16, 32) if name in {"network", "record", "settings"} else (16,)
        for size in sizes:
            key = f"ui_{name}" + ("_large" if size == 32 else "")
            pixels = render(path, size)
            images[key] = pixels
            rows = ["    " + ", ".join(f"0x{value:02x}" for value in pixels[i:i + 16]) + ","
                    for i in range(0, len(pixels), 16)]
            source = (f'/* 由 tools/generate_ui_icons.py 生成的原创图标；8-bit alpha，主题提供颜色。 */\n'
                      '#include "lvgl.h"\n\n'
                      f'static const LV_ATTRIBUTE_MEM_ALIGN uint8_t {key}_alpha[] = {{\n'
                      + "\n".join(rows) + '\n};\n\n'
                      f'const lv_img_dsc_t img_src_{key} = {{\n'
                      f'    .header = {{.cf = LV_IMG_CF_ALPHA_8BIT, .w = {size}, .h = {size}}},\n'
                      f'    .data_size = sizeof({key}_alpha),\n    .data = {key}_alpha,\n}};\n')
            (ROOT / f"img_src_{key}.c").write_text(source)
    # 首页四个图标必须同尺寸且具有接近基准的有效墨量，避免再出现过小、发灰的入口。
    reference = sum(images["ui_info"])
    for name in ("work", "record", "settings"):
        ratio = sum(images[f"ui_{name}"]) / reference
        assert 0.65 < ratio < 1.8, (name, ratio)
    print(f"Generated and checked {len(images)} native-size alpha icons.")


if __name__ == "__main__":
    main()
