# XPrism

[English](README.en.md) | [日本語](README.ja.md) | **简体中文** | [繁體中文](README.zh_TW.md)

## 关于

![appicon](.readme/appicon.png)

清华大学电子工程系硬件设计大赛参赛作品。

遵循 [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html) 许可证。

XPrism 是基于 ESP32-PICO-D4 开发的智能眼镜。

此仓库同时包含基于 .NET MAUI 开发的 Android 端控制器。

Copyright © 2023 [潘徐成](https://github.com/Panxuc) [孙宇琦](https://github.com/always-del)

## 特色

- 使用 ESP32-PICO-KIT 开发板作为主控，使用 ESP32-CAM 模组作为摄像头。
- 可以使用实体按钮、动作识别等多种方式控制。
- 使用 LVGL 图形库，编写了一套简易的 UI 框架；同时制作多款软件程序，满足日常使用需求。
- 与手机端配合，可以实现接收手机通知提醒等多种功能。
- 支持最高 32 GB 的 TF 卡，可以存储大量数据。可以直接打开图片、视频等文件。

## 致谢

XPrism 的诞生离不开以下开源项目的支持、参考与引用。

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
