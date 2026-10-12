"""アイコンを作る。大きく描いてから縮小してなめらかにする。

使い方: python3 tools/make_icon.py
  store/icon_large.png (144x144)  ストア用の大きいアイコン
  store/icon_small.png (80x80)    ストア用の小さいアイコン
  resources/images/menu_icon.png (25x25)  腕時計のメニュー用
"""
import math
from PIL import Image, ImageDraw, ImageFont

SKY = (255, 255, 170)
GROUND = (0, 0, 85)
SUN = (255, 0, 0)
INK = (0, 0, 0)
PAPER = (255, 255, 255)
CURRENT = (255, 85, 0)

BRUSH = 'resources/fonts/brush-kaisho.ttf'
GOTHIC = 'resources/fonts/BIZUDPGothic-Regular.ttf'

# 東京の5月上旬（分）。昼の一刻が長く夜が短い様子を見せつつ、地も十分に残る。
NOON = 11 * 60 + 39
AKE = 4 * 60 + 44
KURE = 18 * 60 + 35
NUMERALS = '六五四九八七六五四九八七'


def boundaries():
    day = (KURE - AKE) / 6
    night = (1440 - (KURE - AKE)) / 6
    return [AKE + day * k for k in range(6)] + [KURE + night * k for k in range(6)]


def point(c, r, minute):
    a = (minute - NOON) / 1440 * 2 * math.pi
    return c + math.sin(a) * r, c - math.cos(a) * r


def draw(size, detail):
    S = size * 8
    im = Image.new('RGBA', (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    c = S / 2
    R = S / 2 - S * 0.02
    disc = R * (0.13 if detail >= 2 else 0.12)
    orbit = R - disc - R * 0.03
    inner = orbit - disc - R * 0.06
    if detail == 0:
        inner = R * 0.42

    rise_y = point(c, orbit, AKE)[1]
    horizon = rise_y

    # 空と地（円の中だけ）
    mask = Image.new('L', (S, S), 0)
    ImageDraw.Draw(mask).ellipse((c - R, c - R, c + R, c + R), fill=255)
    fill = Image.new('RGBA', (S, S), GROUND + (255,))
    ImageDraw.Draw(fill).rectangle((0, 0, S, horizon), fill=SKY + (255,))
    im.paste(fill, (0, 0), mask)
    half = math.sqrt(max(0, R * R - (horizon - c) ** 2))
    d.line((c - half, horizon, c + half, horizon), fill=INK, width=max(1, S // 120))

    b = boundaries()
    # いまの刻の帯（暮れ六つ）
    if detail >= 1:
        start = (b[6] - NOON) / 1440 * 360 - 90
        end = (b[7] - NOON) / 1440 * 360 - 90
        w = disc * 1.9
        d.arc((c - orbit - w / 2, c - orbit - w / 2, c + orbit + w / 2, c + orbit + w / 2),
              start, end, fill=CURRENT, width=int(w))

    # 半刻の点
    if detail >= 1:
        for k in range(12):
            a, z = b[k], b[(k + 1) % 12]
            if z < a:
                z += 1440
            x, y = point(c, orbit, (a + z) / 2)
            r = disc * 0.28
            d.ellipse((x - r, y - r, x + r, y + r), fill=INK if y < horizon else PAPER)

    # 刻の丸
    font = ImageFont.truetype(GOTHIC, int(disc * 1.35)) if detail >= 2 else None
    for k, m in enumerate(b):
        x, y = point(c, orbit, m)
        mutsu = k % 6 == 0
        if detail == 0 and not mutsu:
            continue  # メニュー用の小さな絵では六つだけ残す
        d.ellipse((x - disc, y - disc, x + disc, y + disc),
                  fill=INK if mutsu else PAPER, outline=INK if not mutsu else PAPER,
                  width=max(1, int(disc * 0.12)))
        if font:
            d.text((x, y), NUMERALS[k], font=font, fill=PAPER if mutsu else INK, anchor='mm')

    # 太陽（暮れ六つ半）
    x, y = point(c, orbit, (b[6] + b[7]) / 2)
    r = disc * 0.95
    d.ellipse((x - r, y - r, x + r, y + r), fill=SUN, outline=INK, width=max(1, int(disc * 0.18)))

    # 中央
    d.ellipse((c - inner, c - inner, c + inner, c + inner), fill=PAPER, outline=INK,
              width=max(1, int(R * 0.035)))
    if detail >= 1:
        f = ImageFont.truetype(BRUSH, int(inner * 1.25))
        d.text((c, c + inner * 0.02), '刻', font=f, fill=INK, anchor='mm')

    return im.resize((size, size), Image.LANCZOS)


draw(144, 2).save('store/icon_large.png')
draw(80, 1).save('store/icon_small.png')
draw(25, 0).save('resources/images/menu_icon.png')
