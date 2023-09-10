# Xprism

[English](README.en.md) | **日本語** | [简体中文](README.md) | [繁體中文](README.zh_TW.md)

## このプロジェクトについて

![appicon](.readme/appicon.png)

本作品は清華大学電子工学部のハードウェアデザインコンテストのエントリーです。

[GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html) に基づいてライセンスされています。

XPrism は ESP32-PICO-D4 をベースとしたスマートグラスです。

このリポジトリには .NET MAUI をベースとした Android コントローラーも含まれています。

Copyright © 2023 [潘徐成](https://github.com/Panxuc) [孫宇琦](https://github.com/always-del)

## 特徴

- ESP32-PICO-KIT 開発ボードをメイン コントロールとして使用し、ESP32-CAM モジュールをカメラとして使用します。
- 物理ボタン、モーション認識、その他の方法を使用して制御できます。
- LVGL グラフィック ライブラリを使用して、シンプルな UI フレームワークが作成され、同時に日常の使用ニーズを満たすさまざまなソフトウェア プログラムが作成されました。
- 携帯電話と連携し、携帯電話の通知リマインダーの受信など、さまざまな機能を実現できます。
- 最大32 GBのTFカードをサポートし、大量のデータを保存できます。 写真、ビデオ、その他のファイルを直接開くことができます。

## 謝辞

XPrism の誕生は以下のオープンソースプロジェクトのサポート、参考、引用なしには成り立ちませんでした。

- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [ESP32Time](https://github.com/fbiego/ESP32Time)
- [HoloCubic](https://github.com/peng-zhihui/HoloCubic)
- [HoloCubic_AIO](https://github.com/ClimbSnail/HoloCubic_AIO)
- [i2cdevlib](https://github.com/jrowberg/i2cdevlib)
- [littlefs](https://github.com/littlefs-project/littlefs)
- [lvgl](https://github.com/lvgl/lvgl)
- [MPU6050](https://github.com/kriswiner/MPU6050)
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- [TJpg_Decoder](https://github.com/Bodmer/TJpg_Decoder)
