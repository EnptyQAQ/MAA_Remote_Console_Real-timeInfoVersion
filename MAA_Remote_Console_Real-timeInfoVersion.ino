#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WebSocketsClient.h>
#include "maa_page.h"

// ===== 配置区 =====

// ---- WiFi 配置 ----
const char* WIFI_SSID     = "你的WiFi名称";
const char* WIFI_PASSWORD = "你的WiFi密码";

// ---- 服务器端口 ----
const int SERVER_PORT = 80;

// ---- 串口波特率 ----
const long SERIAL_BAUD = 115200;

// ---- 会话配置 ----
const int MAX_SESSIONS = 3;
const unsigned long SESSION_TIMEOUT_MS = 30UL * 60UL * 1000UL;

// ---- 用户表（用户名 / 密码 / 是否管理员）----
struct User {
  String username;
  String password;
  String maaUser;
  bool   isAdmin;
};

User users[] = {
  {"admin", "admin123", "", true } ,
  {"usERWDFW", "paFSFSSEF", "", false}
};

// ----  Maa-HTTP 电脑 IP 配置 ----
// ⚠️ 运行 Maa-HTTP 的那台电脑的局域网 IP（不是 ESP8266 的 IP）
const char* MAA_HTTP_IP   = "192.168.31.100";  // 运行 Java-HTTP 的电脑局域网 IP
const int   MAA_HTTP_PORT = 8848;              // Java-HTTP 端口，默认 8848
const char* MAA_WS_PATH   = "/API/V1";         // WebSocket 路径，默认不用改

// ----  模拟器配置（参考你 MAA 里面的连接设置） ----
const char* ADB_PATH      = "D:/MAA-v6.16.8-win-x64/adb/platform-tools/adb.exe"; // adb.exe 路径
const char* ADB_HOST      = "127.0.0.1:16384"; // 模拟器 ADB 地址

const int USER_COUNT = sizeof(users) / sizeof(users[0]);

// ============================================================
// ===================== 配置区结束 ===========================
// ============================================================

ESP8266WebServer server(SERVER_PORT);
WebSocketsClient webSocket;

struct UserSession {
  String token;
  int    userIndex;
  unsigned long createdAt;
  bool   active;
};
UserSession sessions[MAX_SESSIONS];

bool maaOn = false;
int activeUserIndex = -1;
String currentMaaConnectId = ""; 

// 非阻塞状态机变量
int maaActionState = 0;
int connectRetry = 0; 
unsigned long lastActionTime = 0;
String tasksParam = ""; 

// 动态接收网页参数
String stageParam = "1-7";
int medParam = 0, stoneParam = 0;
String timesParam = "2147483647";

bool wsConnected = false;
unsigned long lastWsConnectAt = 0;
unsigned long lastMaaMsgTime = 0;

String logMsg1 = "等待操作...";
String logMsg2 = "";
String logMsg3 = "";

String wsLog1 = "";
String wsLog2 = "";
String wsLog3 = "";
String wsLog4 = "";
String wsLog5 = "";

unsigned long lastSessionClean = 0;

void addLog(String msg);
void addWsLog(String msg);
String sendMaaPost(String path, String jsonBody);
String parseMaaCallback(JsonObject dataObj);

// ===================== WebSocket 事件 =====================

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      wsConnected = false;
      addLog("MAA 实时数据源已断开");
      break;

    case WStype_CONNECTED:
      wsConnected = true;
      lastWsConnectAt = millis();
      addLog("已连接到 MAA 实时数据源");
      webSocket.sendTXT("{\"command\":\"callBack\",\"msgId\":0,\"data\":{}}");
      break;

    case WStype_TEXT: {
      String msg = String((char*)payload);
      DynamicJsonDocument doc(3072);
      DeserializationError error = deserializeJson(doc, msg);
      
      if (!error) {
        lastMaaMsgTime = millis(); 
        JsonObject dataObj = doc["data"].as<JsonObject>();
        if (!dataObj.isNull()) {
          String logText = parseMaaCallback(dataObj);
          if (logText != "") addWsLog(logText);
        }
      }
      break;
    }

    default:
      break;
  }
}

// ===================== 深度解析 MAA 回调 =====================

String getTaskChainName(String tc) {
  if (tc == "StartUp") return "开始唤醒";
  if (tc == "CloseDown") return "关闭游戏";
  if (tc == "Fight") return "理智作战";
  if (tc == "Recruit") return "自动公招";
  if (tc == "Infrast") return "基建换班";
  if (tc == "Mall") return "信用收支";
  if (tc == "Award") return "领取奖励";
  return tc;
}

String parseMaaCallback(JsonObject dataObj) {
  int msgId = dataObj["msg"] | 0;
  JsonVariant details = dataObj["details"];
  String logText = "";

  if (msgId > 0 && msgId < 10000) {
    if (details.is<const char*>()) return details.as<String>();
    if (details.is<JsonObject>()) {
       String what = details["what"] | "";
       String why = details["why"] | "";
       logText = what;
       if (why != "" && why != "null") logText += " (" + why + ")";
    }
    return logText != "" ? logText : ("系统通知: " + String(msgId));
  }

  if (!details.is<JsonObject>()) return "";

  if (msgId == 10000 || msgId == 10001 || msgId == 10002 || msgId == 10003 || msgId == 10004) {
    String taskchain = details["taskchain"] | ""; 
    String displayName = getTaskChainName(taskchain); 
    
    if (msgId == 10000) {
      logText = "▶ 开始任务链: " + displayName;
    } 
    else if (msgId == 10001) { 
      logText = "✔ 任务链完成: " + displayName; 
      // 绝对不去删任务，绝对不去改状态
    }
    else if (msgId == 10002) { 
      logText = "⚠ 任务链中断: " + displayName + " (异常)"; 
      // 绝对不去删任务，绝对不去改状态
    }
    else if (msgId == 10003) { 
      logText = "⏹ 任务链已停止: " + displayName; 
    }
    else if (msgId == 10004) {
      logText = "✔ 所有任务已完成，回到待命状态";
      // ★ 唯一真正恢复待命的信号
      maaOn = false;
      activeUserIndex = -1;
    }
    return logText;
  }

  if (msgId == 20000 || msgId == 20001 || msgId == 20002 || msgId == 20003 || msgId == 20004) {
    String what = details["what"] | "";
    String taskchain = getTaskChainName(details["taskchain"] | "");
    
    if (msgId == 20001 || msgId == 20000) {
      if (what != "") return "  ↳ " + what;
      if (details["stage"].is<const char*>()) return "  ↳ 开始作战: " + details["stage"].as<String>();
      return "  ↳ " + taskchain + " 处理中...";
    } 
    else if (msgId == 20002) {
      if (what == "StageDrops") {
        String drops = "  ↳ 作战结算: ";
        if (details["drops"].is<JsonArray>()) {
          for (JsonObject drop : details["drops"].as<JsonArray>()) {
            String item = drop["itemName"] | "";
            int qty = drop["quantity"] | 0;
            if (qty > 0) drops += item + "x" + String(qty) + " ";
          }
        }
        return drops;
      }
      if (what == "StageInfo") return "  ↳ 关卡识别成功: " + String(details["stage"] | "未知");
      return "  ↳ " + taskchain + " 子任务完成";
    } 
    else if (msgId == 20003) return "  ❌ 异常: " + (what != "" ? what : "未知错误");
    else if (msgId == 20004) return "  ⏹ " + taskchain + " 已停止";
  }

  if (msgId == 30000) {
    String what = details["what"] | "";
    if (what == "StageDrops") {
      String drops = "  🎁 掉落: ";
      if (details["drops"].is<JsonArray>()) {
        for (JsonObject drop : details["drops"].as<JsonArray>()) {
          drops += String(drop["itemName"].as<const char*>()) + "x" + String(drop["quantity"].as<int>()) + " ";
        }
      }
      return drops;
    }
    return "  ↳ 信息: " + what;
  }
  return "";
}

// ===================== 工具函数 =====================

String sendMaaPost(String path, String jsonBody) {
  WiFiClient client;
  HTTPClient http;
  String url = "http://" + String(MAA_HTTP_IP) + ":" + String(MAA_HTTP_PORT) + path;
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000); 

  int httpCode = http.POST(jsonBody);
  String response = "";
  if (httpCode == 200) response = http.getString();
  http.end();
  return response;
}

String genToken() {
  String t = "";
  t.reserve(40);
  for (int i = 0; i < 4; i++) {
    t += String(random(0x1000000), HEX);
    if (i < 3) t += "-";
  }
  return t;
}

void addLog(String msg) {
  logMsg3 = logMsg2; logMsg2 = logMsg1;
  unsigned long sec = millis() / 1000;
  char t[20];
  snprintf(t, sizeof(t), "%02u:%02u:%02u", (unsigned)((sec / 3600) % 24), (unsigned)((sec / 60) % 60), (unsigned)(sec % 60));
  logMsg1 = "[" + String(t) + "] " + msg;
}

void addWsLog(String msg) {
  wsLog5 = wsLog4; wsLog4 = wsLog3; wsLog3 = wsLog2; wsLog2 = wsLog1;
  unsigned long sec = millis() / 1000;
  char t[20];
  snprintf(t, sizeof(t), "%02u:%02u:%02u", (unsigned)((sec / 3600) % 24), (unsigned)((sec / 60) % 60), (unsigned)(sec % 60));
  wsLog1 = "[" + String(t) + "] " + msg;
}

void sendJson(int code, const String& body) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(code, "application/json", body);
}

int findSession(const String& tk) {
  if (tk == "") return -1;
  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (!sessions[i].active || sessions[i].token != tk) continue;
    if (millis() - sessions[i].createdAt > SESSION_TIMEOUT_MS) { sessions[i].active = false; return -1; }
    sessions[i].createdAt = millis();
    return i;
  }
  return -1;
}

int createSession(int ui) {
  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (!sessions[i].active) {
      sessions[i].token = genToken(); sessions[i].userIndex = ui;
      sessions[i].createdAt = millis(); sessions[i].active = true; return i;
    }
  }
  int o = 0;
  for (int i = 1; i < MAX_SESSIONS; i++) if (sessions[i].createdAt < sessions[o].createdAt) o = i;
  sessions[o].token = genToken(); sessions[o].userIndex = ui;
  sessions[o].createdAt = millis(); sessions[o].active = true;
  return o;
}

void destroySession(const String& tk) {
  for (int i = 0; i < MAX_SESSIONS; i++)
    if (sessions[i].active && sessions[i].token == tk) { sessions[i].active = false; return; }
}

void cleanExpiredSessions() {
  unsigned long now = millis();
  for (int i = 0; i < MAX_SESSIONS; i++)
    if (sessions[i].active && (now - sessions[i].createdAt > SESSION_TIMEOUT_MS)) sessions[i].active = false;
}

int findUserByName(const String& n) {
  for (int i = 0; i < USER_COUNT; i++) if (users[i].username == n) return i;
  return -1;
}

int currentUser() {
  String tk = server.arg("token");
  int s = findSession(tk);
  return s < 0 ? -1 : sessions[s].userIndex;
}

void handleStaticFile(const String& path, const String& contentType) {
  if (!LittleFS.exists(path)) { server.send(404, "text/plain", "File not found"); return; }
  File file = LittleFS.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
}

void handleRoot() { server.send_P(200, "text/html; charset=utf-8", PAGE_HTML); }

void handleLoginPost() {
  String u = server.arg("username"); String p = server.arg("password");
  int ui = findUserByName(u);
  if (ui < 0 || users[ui].password != p) { addLog("登录失败: " + u); server.send(401, "text/plain", "登录失败"); return; }
  int si = createSession(ui); addLog("用户 " + u + " 已登录");
  server.send(200, "text/plain", sessions[si].token);
}

void handleLogout() {
  String tk = server.arg("token"); int s = findSession(tk);
  if (s >= 0) addLog("用户 " + users[sessions[s].userIndex].username + " 已退出");
  destroySession(tk); server.send(200, "text/plain", "OK");
}

void handleMaaState() {
  int ui = currentUser();
  if (ui < 0) { server.send(401, "text/plain", "未登录"); return; }
  
  DynamicJsonDocument doc(1536);
  doc["on"] = maaOn; 
  doc["actionState"] = maaActionState; 
  doc["log1"] = logMsg1; doc["log2"] = logMsg2; doc["log3"] = logMsg3;
  doc["wsLog1"] = wsLog1; doc["wsLog2"] = wsLog2; doc["wsLog3"] = wsLog3; doc["wsLog4"] = wsLog4; doc["wsLog5"] = wsLog5;
  doc["username"] = users[ui].username; 
  doc["isAdmin"] = users[ui].isAdmin; 
  doc["wsConnected"] = wsConnected;
  doc["myIndex"] = ui; doc["activeUserIndex"] = activeUserIndex;
  doc["activeUser"] = (activeUserIndex >= 0) ? users[activeUserIndex].username : "";
  
  String out; serializeJson(doc, out); sendJson(200, out);
}

void handleMaaSet() {
  int ui = currentUser();
  if (ui < 0) { server.send(401, "text/plain", "未登录"); return; }
  String s = server.arg("state");
  
  if (s != "on" && s != "off") { server.send(400, "text/plain", "参数错误"); return; }
  if (activeUserIndex != -1 && activeUserIndex != ui) { server.send(409, "text/plain", "其他用户正在控制"); return; }

  activeUserIndex = ui; 
  server.send(200, "text/plain", "OK"); 

  if (s == "on") {
    maaOn = true;
    tasksParam = server.arg("tasks");
    if (tasksParam == "") {
      server.send(400, "text/plain", "未选择任务");
      maaOn = false;
      return;
    }

    stageParam = server.arg("stage");
    if (stageParam == "") stageParam = "1-7"; 
    medParam = server.arg("med").toInt();
    stoneParam = server.arg("stone").toInt();
    timesParam = server.arg("times");
    if (timesParam == "") timesParam = "2147483647"; 
    
    addLog("用户 " + users[ui].username + " 请求开启任务");
    maaActionState = 1; 
    connectRetry = 0; 
    lastActionTime = millis();
    lastMaaMsgTime = millis(); 
  } else {
    maaOn = false; 
    maaActionState = 0; 
    addLog("用户 " + users[ui].username + " 请求停止当前任务");
    if (currentMaaConnectId != "") {
      sendMaaPost("/API/V1/stop", "{\"id\":\"" + currentMaaConnectId + "\"}");
      currentMaaConnectId = ""; 
      addLog("停止指令已送达 MAA，已重置连接");
    }
    activeUserIndex = -1;
  }
}

void handleAdminData() {
  int ui = currentUser();
  if (ui < 0 || !users[ui].isAdmin) { server.send(403, "text/plain", "无权限"); return; }
  DynamicJsonDocument doc(3072);
  unsigned long sec = millis() / 1000;
  doc["ip"] = WiFi.localIP().toString(); doc["uptime"] = String(sec / 3600) + "h " + String((sec / 60) % 60) + "m " + String(sec % 60) + "s";
  doc["heap"] = String(ESP.getFreeHeap() / 1024) + " KB";
  String u = "";
  for (int i = 0; i < USER_COUNT; i++) { u += "• " + users[i].username + (users[i].isAdmin ? " [管理员]" : "") + "\n"; }
  doc["users"] = u;
  int online = 0; String ss = "";
  for (int i = 0; i < MAX_SESSIONS; i++) { if (sessions[i].active) { online++; ss += "• " + users[sessions[i].userIndex].username + "\n"; } }
  if (ss == "") ss = "无\n";
  doc["online"] = online; doc["sessions"] = ss;
  doc["activeUser"] = (activeUserIndex >= 0) ? users[activeUserIndex].username : "无";
  doc["on"] = maaOn; doc["wsConnected"] = wsConnected;
  doc["logs"] = logMsg1 + "\n" + logMsg2 + "\n" + logMsg3 + "\n";
  String out; serializeJson(doc, out); sendJson(200, out);
}

void setup() {
  Serial.begin(SERIAL_BAUD); delay(500);
  randomSeed(ESP.getChipId() * micros());
  if (!LittleFS.begin()) { LittleFS.format(); LittleFS.begin(); }
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("连接 WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n控制台: http://" + WiFi.localIP().toString() + "/");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_POST, handleLoginPost);
  server.on("/logout", HTTP_GET, handleLogout);
  server.on("/maa/state", HTTP_GET, handleMaaState);
  server.on("/maa/set", HTTP_GET, handleMaaSet);
  server.on("/admin/data", HTTP_GET, handleAdminData);
  server.on("/avatar.jpg", HTTP_GET, []() { handleStaticFile("/avatar.jpg", "image/jpeg"); });
  server.on("/avatar.png", HTTP_GET, []() { handleStaticFile("/avatar.png", "image/png"); });

  server.begin();
  Serial.println("HTTP 服务器已启动，正在连接 Maa-HTTP WebSocket...");
  webSocket.begin(MAA_HTTP_IP, MAA_HTTP_PORT, MAA_WS_PATH);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(10000);
  webSocket.enableHeartbeat(15000, 3000, 2); 
}

void loop() {
  server.handleClient();
  webSocket.loop();

  if (maaActionState > 0 && millis() - lastActionTime > 300) {
    lastActionTime = millis();
    maaOn = true; 
    
    if (maaActionState == 1) {
      String connectBody = "{\"adbPath\":\"" + String(ADB_PATH) + "\",\"host\":\"" + String(ADB_HOST) + "\",\"detailJson\":\"\"}";
      String connectResp = sendMaaPost("/API/V1/connect", connectBody);
      
      int idIndex = connectResp.indexOf("\"id\":\"");
      if (idIndex >= 0) {
        int endIndex = connectResp.indexOf("\"", idIndex + 6);
        if (endIndex > idIndex) {
          currentMaaConnectId = connectResp.substring(idIndex + 6, endIndex);
        }
      }

      if (currentMaaConnectId != "") {
        connectRetry = 0;
        maaActionState = 2; 
      } else {
        connectRetry++;
        if (connectRetry >= 3) {
          addLog("MAA 连接失败，请检查模拟器或网络");
          maaOn = false; activeUserIndex = -1; maaActionState = 0;
          connectRetry = 0;
        } else {
          addLog("连接模拟器超时，正在重试 (" + String(connectRetry) + "/3)...");
        }
      }
    } 
    else if (maaActionState == 2) {
      if (tasksParam.indexOf("StartUp") >= 0) {
        sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"StartUp\",\"params\":{\"client_type\":\"Official\",\"start_game_enabled\":true}}");
      }
      maaActionState = 3;
    }
    else if (maaActionState == 3) {
      if (tasksParam.indexOf("Fight") >= 0) {
        String fightBody = "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Fight\",\"params\":{\"stage\":\"" + stageParam + "\",\"medicine\":" + String(medParam) + ",\"stone\":" + String(stoneParam) + ",\"times\":" + timesParam + "}}";
        sendMaaPost("/API/V1/appendTask", fightBody);
      }
      maaActionState = 4;
    }
    else if (maaActionState == 4) {
      if (tasksParam.indexOf("Infrast") >= 0) {
        // 基建队列轮换模式
        sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Infrast\",\"params\":{\"mode\":20000,\"facility\":[\"Mfg\",\"Trade\",\"Control\",\"Power\",\"Reception\",\"Office\",\"Dorm\",\"Processing\",\"Training\"],\"drones\":\"Money\"}}");
      }
      maaActionState = 5;
    }
    else if (maaActionState == 5) {
      if (tasksParam.indexOf("Recruit") >= 0) {
        sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Recruit\",\"params\":{\"select\":[4],\"confirm\":[3,4],\"times\":4,\"refresh\":true,\"force_refresh\":true}}");
      }
      maaActionState = 6;
    }
    else if (maaActionState == 6) {
      if (tasksParam.indexOf("Mall") >= 0) {
        sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Mall\",\"params\":{\"shopping\":true,\"buy_first\":[\"招聘许可\"],\"blacklist\":[\"碳\",\"家具\",\"加急许可\"],\"visit_friends\":true}}");
      }
      maaActionState = 7;
    }
    else if (maaActionState == 7) {
      if (tasksParam.indexOf("Award") >= 0) {
        sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Award\",\"params\":{\"award\":true,\"mail\":true,\"free_gacha\":true,\"orundum\":true,\"mining\":true,\"specialaccess\":true,\"sign_in_event\":false}}");
      }
      maaActionState = 8;
    }
    else if (maaActionState == 8) {
      sendMaaPost("/API/V1/start", "{\"id\":\"" + currentMaaConnectId + "\"}");
      addLog("所有勾选任务已按顺序送达 MAA");
      maaActionState = 0; 
    }
  }

  // 静默看门狗：5 分钟内没有任何事件，自动重置
  if (maaOn && maaActionState == 0) {
    if (millis() - lastMaaMsgTime > 300000) {
      addLog("超过5分钟未收到MAA消息，自动待命");
      maaOn = false;
      activeUserIndex = -1;
    }
  }

  if (millis() - lastSessionClean > 60000UL) {
    lastSessionClean = millis();
    cleanExpiredSessions();
  }
}
