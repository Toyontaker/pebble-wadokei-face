# Wadokei — ストア掲載用テキスト

Rebble アプリストアや配布ページに貼り付ける用の文面です。日本語と英語を用意しています。

---

## タイトル

Wadokei 和時計

## 一行紹介（日本語）

日の出・日の入りから昼と夜を六等分する、江戸の不定時法のウォッチフェイス。

## 一行紹介（English）

A Japanese temporal-hour watchface: day and night are each split into six, so the hours stretch and shrink with the seasons.

---

## 説明文（日本語）

江戸時代の時刻「不定時法」で時を示すウォッチフェイスです。
明け六つから暮れ六つまでの昼と、暮れ六つから翌朝の明け六つまでの夜を、それぞれ六つの「刻」に分けます。いまが「暮れ六つ」なのか「夜九つ」なのか、干支とあわせて表示します。

一刻の長さは季節によって伸び縮みします。東京では夏至の昼の一刻が約2時間26分、夜は約1時間34分。冬至になると昼が約1時間37分、夜が約2時間23分と逆転します。日の出・日の入りは緯度経度から時計の上で毎日計算するので、場所と季節に合った刻がわかります。

文字盤は24時間で一周する太陽の軌道です。南中が真上に来るよう毎日回り、日の出と日の入りを結ぶ地平線で空と地を塗り分けます。外周に並ぶ刻の丸は、昼側では間隔が広く夜側では狭く（冬は逆に）なり、一刻の長さがそのまま目に見えます。赤い丸が今の太陽、橙の帯が今いる刻です。

- 中央に干支（酉の刻）、刻の名（明け六つ・昼九つ・暮れ六つ…。刻の後半は「暮れ六つ半」）、今の時刻、今の一刻の長さ
- 長方形の画面では、四隅に日付・曜日と、今日の昼・夜それぞれの一刻の長さ
- 昼と夜の境目は「日の出・日の入り」と、江戸の寛政暦にならった「太陽が地平線下 7°21′40″（日の出の約36分前・日の入りの約36分後）」から選べます
- 場所はスマホの位置情報か、手動で入力した緯度・経度。位置を一度受け取れば時計に保存され、スマホと離れていても毎日計算し直します

刻の名は筆文字で表示します。日本語フォントをアプリに同梱しているので、時計に言語パックが入っていなくても表示できます。設定画面はスマホの言語に合わせて日本語か英語で表示します。

### プライバシー

- 位置情報は、日の出・日の入りを計算するためだけに時計へ送ります。外部のサーバーには一切送りません
- 開発者側のサーバーはなく、通信はスマホと時計の間だけです

### 対応機種

Pebble Time / Time Steel（basalt）、Pebble Time Round（chalk）、Pebble 2（diorite）、Pebble Time 2（emery）、Pebble 2 Duo（flint）、Pebble Round 2（gabbro）

### ライセンスとクレジット

- 本作は [クリエイティブ・コモンズ 表示 - 継承 4.0 国際（CC BY-SA 4.0）](https://creativecommons.org/licenses/by-sa/4.0/deed.ja) で公開します。
- 文字盤のデザインは JR Mobley さんの Horizon Watch Face（© 2015, 2016 JR Mobley、CC BY-SA 4.0、https://github.com/jrmobley/horizon-watchface ）を元にしています。目盛りを不定時法の刻に置き換え、日の出・日の入りの計算を時計側に移し、日本語表示を加えるなどの改変をしています。
- フォント：Yuji Syuku（© The Yuji Project Authors）、BIZ UDPGothic（© Google）。いずれも SIL Open Font License 1.1
- 日の出・日の入りの計算式：NOAA Global Monitoring Laboratory

---

## Description (English)

A watchface that tells time the way Edo-period Japan did, with temporal hours (futeijihō).
Daytime, from dawn (Ake-mutsu) to dusk (Kure-mutsu), and night, from dusk to the next dawn, are each divided into six "koku". The face shows which koku it is now, such as Kure-mutsu (dusk) or Yo-kokonotsu (midnight), together with its zodiac sign.

Because day and night are always split into six, the length of a koku changes with the seasons. In Tokyo a daytime koku lasts about 2 h 26 min at the summer solstice and a night koku about 1 h 34 min. At the winter solstice they swap: about 1 h 37 min by day and 2 h 23 min at night. The watch calculates sunrise and sunset from your latitude and longitude every day, so the koku always match your place and season.

The dial is the sun's 24-hour path. It turns each day so that solar noon sits at the top, and the horizon line drawn between sunrise and sunset divides sky from ground. The koku markers around the edge are spaced wider on the day side and closer on the night side (the other way round in winter), so you can see the length of a koku at a glance. The red disc is the sun right now, and the orange band marks the current koku.

- In the center: zodiac sign, koku name (with "han" for its second half), the current time, and the length of the current koku
- On rectangular screens, the corners show the date and weekday, and today's daytime and night koku lengths
- Day and night can begin at sunrise and sunset, or follow the Edo-period Kansei calendar rule: the sun 7°21′40″ below the horizon (about 36 minutes before sunrise and after sunset)
- Location comes from your phone or from coordinates you enter. Once received, it is stored on the watch, which recalculates every day even when your phone is away

The watchface itself is in Japanese: koku names are drawn in a brush-style font. The Japanese fonts are bundled with the app, so no language pack is needed. The settings page appears in English or Japanese, following your phone's language.

### Privacy

- Your location is sent only to your watch, to calculate sunrise and sunset. It is never sent to any server
- There is no developer server. All communication stays between your phone and your watch

### Supported watches

Pebble Time / Time Steel (basalt), Pebble Time Round (chalk), Pebble 2 (diorite), Pebble Time 2 (emery), Pebble 2 Duo (flint), Pebble Round 2 (gabbro)

### License and credits

- This watchface is released under the [Creative Commons Attribution-ShareAlike 4.0 International License (CC BY-SA 4.0)](https://creativecommons.org/licenses/by-sa/4.0/).
- The dial design is based on Horizon Watch Face by JR Mobley (© 2015, 2016 JR Mobley, CC BY-SA 4.0, https://github.com/jrmobley/horizon-watchface). Changes include replacing the hour marks with temporal-hour koku, moving the sunrise and sunset calculation onto the watch, and adding Japanese text.
- Fonts: Yuji Syuku (© The Yuji Project Authors) and BIZ UDPGothic (© Google), both under the SIL Open Font License 1.1
- Sunrise and sunset formula: NOAA Global Monitoring Laboratory

---

## 画像

| ファイル | 用途 |
|---|---|
| `store/icon_large.png` | 大きいアイコン（144×144） |
| `store/icon_small.png` | 小さいアイコン（80×80） |
| `screenshots/*.png` | スクリーンショット |

アイコンは `python3 tools/make_icon.py` で作り直せます（腕時計のメニュー用 `resources/images/menu_icon.png` も同時に出力）。
