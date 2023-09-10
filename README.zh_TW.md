# XPrism

[English](README.en.md) | [日本語](README.ja.md) | [简体中文](README.md) | **繁體中文**

## 關於

![appicon](.readme/appicon.png)

清華大學電子工程系硬體設計大賽參賽作品。

遵循 [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html) 授權條款。

XPrism 是基於 ESP32-PICO-D4 開發的智慧眼鏡。

此倉庫同時包含基於 .NET MAUI 開發的 Android 端控制器。

Copyright © 2023 [潘徐成](https://github.com/Panxuc) [孫宇琦](https://github.com/always-del)

## 特色

- 使用 ESP32-PICO-KIT 開發板作為主控，使用 ESP32-CAM 模組作為攝像頭。
- 可以使用實體按鈕、動作識別等多種方式控制。
- 使用 LVGL 圖形庫，編寫了一套簡易的 UI 框架；同時製作多款軟體程式，滿足日常使用需求。
- 與手機端配合，可以實現接收手機通知提醒等多種功能。
- 支援最高 32 GB 的 TF 卡，可以儲存大量資料。可以直接開啟圖片、影片等檔案。

## 致謝

XPrism 的誕生離不開以下開源專案的支援、參考與引用。

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