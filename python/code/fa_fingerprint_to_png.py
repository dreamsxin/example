#!/usr/bin/env python3
"""
fa_fingerprint_to_png.py
将 Font Awesome Solid 的「指纹」图标渲染成多种尺寸的 PNG。

依赖:
    pip install Pillow fonttools requests

用法:
    python fa_fingerprint_to_png.py                        # 默认输出到 ./icons/
    python fa_fingerprint_to_png.py --color "#4F46E5"      # 自定义图标颜色（十六进制）
    python fa_fingerprint_to_png.py --bg "#FFFFFF"         # 自定义背景色（默认透明）
    python fa_fingerprint_to_png.py --out ./output/        # 自定义输出目录
    python fa_fingerprint_to_png.py --font /path/to/fa.ttf # 指定本地字体文件
"""

import argparse
import os
import sys
import urllib.request
from pathlib import Path
from typing import List, Optional, Tuple

# ── 依赖检查 ───────────────────────────────────────────────────────────────────
try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    sys.exit("缺少 Pillow，请执行: pip install Pillow")

try:
    from fontTools.ttLib import TTFont
except ImportError:
    sys.exit("缺少 fonttools，请执行: pip install fonttools")

# ── 常量 ───────────────────────────────────────────────────────────────────────
# Font Awesome 6 Free Solid — jsDelivr CDN（稳定，国内可访问）
FA_FONT_URL = (
    "https://cdn.jsdelivr.net/npm/@fortawesome/fontawesome-free@6.5.2"
    "/webfonts/fa-solid-900.ttf"
)
# 指纹图标的 Unicode 码点（fa-fingerprint = U+F577）
FINGERPRINT_CODEPOINT = 0xF577

# 默认输出尺寸列表
DEFAULT_SIZES = [16, 32, 48, 64, 128, 192, 256, 512]

# ── 辅助函数 ──────────────────────────────────────────────────────────────────

def download_font(dest: Path) -> Path:
    """下载 Font Awesome Solid 字体到本地缓存。"""
    dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.exists():
        print(f"[缓存] 已有字体文件: {dest}")
        return dest
    print(f"[下载] 正在下载 Font Awesome 字体...")
    print(f"       URL: {FA_FONT_URL}")
    try:
        urllib.request.urlretrieve(FA_FONT_URL, dest)
        print(f"[完成] 字体已保存到: {dest}")
    except Exception as e:
        sys.exit(f"下载失败: {e}\n请手动下载字体文件并通过 --font 参数指定路径。")
    return dest


def verify_glyph(font_path: Path) -> bool:
    """确认字体文件包含指纹字符（U+F577）。"""
    try:
        tt = TTFont(str(font_path))
        cmap = tt.getBestCmap()
        return cmap is not None and FINGERPRINT_CODEPOINT in cmap
    except Exception as e:
        print(f"[警告] 字体验证失败: {e}")
        return False


def parse_color(hex_str: str) -> tuple:
    """将 #RRGGBB 或 #RRGGBBAA 解析为 RGBA 元组。"""
    h = hex_str.lstrip("#")
    if len(h) == 6:
        r, g, b = int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16)
        return (r, g, b, 255)
    elif len(h) == 8:
        r, g, b, a = int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16), int(h[6:8], 16)
        return (r, g, b, a)
    else:
        sys.exit(f"颜色格式错误: {hex_str}，请使用 #RRGGBB 或 #RRGGBBAA")


def render_icon(font_path: Path, size: int, icon_color: Tuple,
                bg_color: Optional[Tuple], padding_ratio: float = 0.1) -> Image.Image:
    """
    将指纹图标渲染为指定尺寸的 PIL Image（RGBA）。

    :param font_path:     字体文件路径
    :param size:          目标输出尺寸（正方形，单位 px）
    :param icon_color:    图标 RGBA 颜色
    :param bg_color:      背景 RGBA 颜色，None = 透明
    :param padding_ratio: 图标四周留白比例（占画布边长）
    """
    # 创建画布
    mode = "RGBA"
    canvas_bg = bg_color if bg_color else (0, 0, 0, 0)
    img = Image.new(mode, (size, size), canvas_bg)
    draw = ImageDraw.Draw(img)

    # 计算字体大小：先用 padding 缩小可用区，再对齐字符实际包围盒
    padding = int(size * padding_ratio)
    available = size - 2 * padding

    # 二分法：找到让字符恰好填满 available 的字体大小
    char = chr(FINGERPRINT_CODEPOINT)
    lo, hi = 1, available * 2
    best_font = None
    best_size_px = 1

    while lo <= hi:
        mid = (lo + hi) // 2
        try:
            font = ImageFont.truetype(str(font_path), mid)
        except IOError:
            break
        bbox = font.getbbox(char)            # (left, top, right, bottom)
        w = bbox[2] - bbox[0]
        h = bbox[3] - bbox[1]
        if max(w, h) <= available:
            best_font = font
            best_size_px = mid
            lo = mid + 1
        else:
            hi = mid - 1

    if best_font is None:
        best_font = ImageFont.truetype(str(font_path), available)

    # 居中绘制
    bbox = best_font.getbbox(char)
    w = bbox[2] - bbox[0]
    h = bbox[3] - bbox[1]
    x = (size - w) // 2 - bbox[0]
    y = (size - h) // 2 - bbox[1]

    draw.text((x, y), char, font=best_font, fill=icon_color)
    return img


def generate_icons(
    font_path: Path,
    out_dir: Path,
    sizes: List[int],
    icon_color: Tuple,
    bg_color: Optional[Tuple],
):
    """批量生成各尺寸 PNG。"""
    out_dir.mkdir(parents=True, exist_ok=True)
    results = []

    for size in sizes:
        img = render_icon(font_path, size, icon_color, bg_color)
        filename = f"fingerprint_{size}x{size}.png"
        dest = out_dir / filename
        img.save(dest, "PNG")
        results.append(dest)
        print(f"  [OK] {filename}")

    return results


# ── 主程序 ────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="将 Font Awesome 指纹图标渲染为多种尺寸 PNG"
    )
    parser.add_argument(
        "--color", default="#4F46E5",
        help="图标颜色（十六进制，默认: #4F46E5 indigo）"
    )
    parser.add_argument(
        "--bg", default=None,
        help="背景颜色（十六进制，默认: 透明）"
    )
    parser.add_argument(
        "--out", default="icons",
        help="PNG 输出目录（默认: ./icons/）"
    )
    parser.add_argument(
        "--font", default=None,
        help="本地 Font Awesome Solid TTF/OTF 路径（跳过下载）"
    )
    parser.add_argument(
        "--sizes", default=None,
        help=f"逗号分隔的尺寸列表（默认: {','.join(map(str, DEFAULT_SIZES))}）"
    )
    parser.add_argument(
        "--padding", type=float, default=0.1,
        help="图标留白比例 0.0~0.3（默认: 0.1）"
    )

    args = parser.parse_args()

    # 解析尺寸
    if args.sizes:
        try:
            sizes = [int(s.strip()) for s in args.sizes.split(",")]
        except ValueError:
            sys.exit("--sizes 格式错误，示例: --sizes 16,32,64,128")
    else:
        sizes = DEFAULT_SIZES

    # 解析颜色
    icon_color = parse_color(args.color)
    bg_color = parse_color(args.bg) if args.bg else None

    # 字体路径
    if args.font:
        font_path = Path(args.font)
        if not font_path.exists():
            sys.exit(f"字体文件不存在: {font_path}")
    else:
        cache_dir = Path(__file__).parent / ".fa_cache"
        font_path = cache_dir / "fa-solid-900.ttf"
        font_path = download_font(font_path)

    # 验证字体包含指纹字符
    print("[验证] 检查字体是否包含指纹字符 (U+F577)...", end=" ")
    if verify_glyph(font_path):
        print("OK")
    else:
        print("FAIL")
        sys.exit("字体文件不包含指纹字符，请确认使用的是 Font Awesome Solid 字体。")

    # 生成图标
    out_dir = Path(args.out)
    print(f"\n[生成] 输出目录: {out_dir.resolve()}")
    print(f"       图标颜色: {args.color}  背景: {'透明' if bg_color is None else args.bg}")
    print(f"       尺寸列表: {sizes}\n")

    results = generate_icons(font_path, out_dir, sizes, icon_color, bg_color)

    print(f"\n[完成] 共生成 {len(results)} 个 PNG 文件，保存在: {out_dir.resolve()}")


if __name__ == "__main__":
    main()
