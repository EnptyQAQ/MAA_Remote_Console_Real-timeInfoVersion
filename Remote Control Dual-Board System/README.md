# MAA_Remote_Console_Real-timeInfoVersion
基于 ESP8266 和 [MaaAssistantArknights (MAA)](https://github.com/MaaAssistantArknights/MaaAssistantArknights) [MAA-RC](https://github.com/EnptyQAQ/MAA-Remote-Console)的远程可视化控制台
通过本设备，你可以直接在手机或电脑的网页上控制局域网内的 MAA 启动各项任务，并**实时查看 MAA 的运行日志和掉落信息**。

## ✨ 功能特性
- 📱 **全平台适配**：手机端单列，电脑端双列宽屏排版。
- 🚀 **WebSocket 实时日志**：即时查看 MAA 的作战进度、掉落物品等。
- ⚙️ **任务自由勾选**：支持开始唤醒、理智作战、基建换班、自动公招、信用收支、领取奖励。
- ⚔️ **参数动态配置**：可在网页端直接修改关卡（如 1-7）、吃药、碎石次数。
- 👥 **多用户支持**：内置简单的会话管理，支持不同用户登录操作（防止同时下发指令冲突）。

## 🛠️ 准备工作与依赖
在 Arduino IDE 中，需要安装以下库：
- `ESP8266` (核心包 >= 3.1.2)
- `ArduinoJson` (v6.x)
- `WebSockets` (由 Markus Sattler 提供)
- `ESP8266HTTPClient` (核心包自带)

## 🚀 快速开始

### 1. 修改配置
打开 `MAA_Remote_Console_Real-timeInfoVersion.ino`，在顶部的【配置区】修改：
- `WIFI_SSID` / `WIFI_PASSWORD`：你的局域网 WiFi 账号密码。
- `MAA_HTTP_IP`：运行 `Maa-HTTP` 的电脑局域网 IP。
- `ADB_PATH` / `ADB_HOST`：你的模拟器 adb 路径和地址（与 MAA 内配置一致）。

### 2. 烧录
使用 Arduino IDE 或 VSCode (PlatformIO) 将代码烧录至 ESP8266 (如 NodeMCU)。

### 3. ☕ 启动 Maa-HTTP (电脑端)

**⚠️ 极其重要：**
本项目是直接通过 HTTP 和 WebSocket 与 MAA 的底层核心 (`MaaCore`) 进行通信的。因此，**你不需要打开 MAA 的图形界面 (GUI)**，更**不要**在 MAA GUI 中开启“远程控制”功能。你只需要在电脑上运行 `Maa-HTTP` 这个独立进程即可。

 1. 前置准备
- 确保电脑上已安装 **Java 运行环境** (JRE 11 或更高版本)。
- 确保你的**安卓模拟器已经启动**，并且游戏处于主界面。
- 准备好 MAA 的完整本体文件夹（主要用于提供 `resource` 资源文件）。

 2. 获取并放置 Maa-HTTP
    你需要获取 `Maa-HTTP` 的 `.jar` 包（如果你是自己编译的，请确保打包完好）。
    将下载好的 `Maa-HTTP.jar` 放置在 **MAA 的根目录下**（与 `MaaCore.dll` 和 `resource` 文件夹同级）。

 4. 修改 Maa-HTTP 配置（关键！）
    默认情况下，Maa-HTTP 可能只监听 `127.0.0.1`（仅限本机访问）。为了让你的 ESP8266 能连上它，必须让它监听局域网。
    在 MAA 根目录下新建或修改 `config.json`（或者通过启动参数），确保其监听地址为 `0.0.0.0`。
    5. 启动命令
       打开电脑的 CMD（命令提示符），进入 MAA 的根目录，执行以下命令启动服务：
       请注意替换为你实际的文件名
       java(你的java路径) -jar Maa-HTTP-0.0.1.jar
### 4. 访问控制台
查看串口监视器，获取 ESP8266 的 IP 地址，在浏览器中打开即可（例如 `http://192.168.1.100`）。

## 📄 开源协议
本项目基于 CC BY-NC-ND 4.0 协议开源 不允许商业用 如果有人商业用可以联系ENPTY@qq.com
