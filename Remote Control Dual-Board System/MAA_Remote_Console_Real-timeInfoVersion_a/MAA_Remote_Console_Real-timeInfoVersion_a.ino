#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <SoftwareSerial.h>
#include "maa_page.h"

// 软串口：GPIO12(D6) 为 RX，GPIO14(D5) 为 TX
SoftwareSerial toB(12, 14); 

// ============================================================
// =================== ⚙️ 配置区 ===================
// ============================================================
const char* WIFI_SSID     = "你的WiFi名称";
const char* WIFI_PASSWORD = "你的WiFi密码";

const int   SERVER_PORT   = 80;

const int MAX_SESSIONS  = 3;
const unsigned long SESSION_TIMEOUT_MS = 30UL * 60UL * 1000UL;

struct User { String username; String password; bool isAdmin; };
User users[] = {
 {"admin", "admin123", "", true } ,
  {"usERWDFW", "paFSFSSEF", "", false}
};
const int USER_COUNT = sizeof(users) / sizeof(users[0]);
// ============================================================

ESP8266WebServer server(SERVER_PORT);

struct UserSession { String token; int userIndex; unsigned long createdAt; bool active; };
UserSession sessions[MAX_SESSIONS];

bool maaOn = false;
bool wsConnected = false;
int activeUserIndex = -1;

String logMsg1 = "等待操作...";
String logMsg2 = "";
String logMsg3 = "";

String wsLog1 = "";
String wsLog2 = "";
String wsLog3 = "";

unsigned long lastSessionClean = 0;
String serialBuffer = "";

bool bBoardOnline = false;
unsigned long lastBPingTime = 0;
int bBoardHeap = 0; // 记录 B 板可用内存

void addLog(String msg) {
  logMsg3 = logMsg2; logMsg2 = logMsg1;
  unsigned long sec = millis() / 1000;
  char t[20];
  snprintf(t, sizeof(t), "%02u:%02u:%02u", (unsigned)((sec / 3600) % 24), (unsigned)((sec / 60) % 60), (unsigned)(sec % 60));
  logMsg1 = "[" + String(t) + "] " + msg;
}

void addWsLog(String msg) {
  wsLog3 = wsLog2; wsLog2 = wsLog1;
  unsigned long sec = millis() / 1000;
  char t[20];
  snprintf(t, sizeof(t), "%02u:%02u:%02u", (unsigned)((sec / 3600) % 24), (unsigned)((sec / 60) % 60), (unsigned)(sec % 60));
  wsLog1 = "[" + String(t) + "] " + msg;
}

void processSerial() {
  while (toB.available()) {
    char c = toB.read();
    if (c == '\n') {
      serialBuffer.trim();
      if (serialBuffer.startsWith("LOG:")) {
        addWsLog(serialBuffer.substring(4));
      } else if (serialBuffer.startsWith("STATE:")) {
        maaOn = (serialBuffer.substring(6) == "1");
        if (!maaOn) activeUserIndex = -1;
      } else if (serialBuffer.startsWith("SYS:")) {
        addLog(serialBuffer.substring(4));
      } else if (serialBuffer.startsWith("WS:")) {
        wsConnected = (serialBuffer.substring(3) == "1");
      } else if (serialBuffer.startsWith("PING:")) {
        bBoardOnline = true;
        lastBPingTime = millis();
        bBoardHeap = serialBuffer.substring(5).toInt();
      } else if (serialBuffer == "PING") {
        bBoardOnline = true;
        lastBPingTime = millis();
      }
      serialBuffer = "";
    } else {
      serialBuffer += c;
      if (serialBuffer.length() > 256) serialBuffer = ""; 
    }
  }
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
    return i;
  }
  return -1;
}

int createSession(int ui) {
  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (!sessions[i].active) {
      sessions[i].token = String(random(0xFFFFFF), HEX) + String(random(0xFFFFFF), HEX);
      sessions[i].userIndex = ui; sessions[i].createdAt = millis(); sessions[i].active = true; return i;
    }
  }
  return -1;
}

int currentUser() {
  int s = findSession(server.arg("token"));
  return s < 0 ? -1 : sessions[s].userIndex;
}

void handleRoot() { server.send_P(200, "text/html; charset=utf-8", PAGE_HTML); }

void handleLoginPost() {
  String u = server.arg("username"); String p = server.arg("password");
  for (int i = 0; i < USER_COUNT; i++) {
    if (users[i].username == u && users[i].password == p) {
      int si = createSession(i); addLog("用户 " + u + " 登录"); server.send(200, "text/plain", sessions[si].token); return;
    }
  }
  server.send(401, "text/plain", "失败");
}

void handleLogout() {
  int s = findSession(server.arg("token"));
  if (s >= 0) sessions[s].active = false;
  server.send(200, "text/plain", "OK");
}

void handleMaaState() {
  int ui = currentUser();
  if (ui < 0) { server.send(401, "text/plain", "未登录"); return; }
  DynamicJsonDocument doc(1024);
  doc["on"] = maaOn; doc["log1"] = logMsg1; doc["log2"] = logMsg2; doc["log3"] = logMsg3;
  doc["wsLog1"] = wsLog1; doc["wsLog2"] = wsLog2; doc["wsLog3"] = wsLog3;
  doc["username"] = users[ui].username; doc["isAdmin"] = users[ui].isAdmin; doc["wsConnected"] = wsConnected;
  doc["bBoardOnline"] = bBoardOnline;
  doc["myIndex"] = ui; doc["activeUserIndex"] = activeUserIndex;
  doc["activeUser"] = (activeUserIndex >= 0) ? users[activeUserIndex].username : "";
  // ★ 新增：前端状态栏依赖的两个字段
  doc["pending"]     = (maaOn && activeUserIndex >= 0 && activeUserIndex != ui);
  doc["actionState"] = maaOn ? -1 : 0;  // -1 = 运行中（阶段未知），0 = 待命
  String out; serializeJson(doc, out); sendJson(200, out);
}

void handleMaaSet() {
  int ui = currentUser();
  if (ui < 0) { server.send(401, "text/plain", "未登录"); return; }
  String s = server.arg("state");
  
  if (s == "on") {
    if (activeUserIndex != -1 && activeUserIndex != ui) { server.send(409, "text/plain", "其他用户控制中"); return; }
    activeUserIndex = ui; maaOn = true;
    
    // ★ 使用竖线 | 作为分隔符，避免和任务列表的逗号冲突
    String cmd = "START:" + server.arg("tasks") + "|" + server.arg("stage") + "|" + server.arg("series") + "|" + server.arg("med") + "|" + server.arg("stone") + "|" + server.arg("times") + "\n";
    
    toB.print(cmd); 
    addLog("指令已下发至副板");
  } else {
    maaOn = false; activeUserIndex = -1;
    toB.println("STOP"); 
    addLog("停止指令已下发至副板");
  }
  server.send(200, "text/plain", "OK");
}

void handleAdminData() {
  int ui = currentUser();
  if (ui < 0 || !users[ui].isAdmin) { server.send(403, "text/plain", "无权限"); return; }
  DynamicJsonDocument doc(2048);
  doc["aHeap"] = String(ESP.getFreeHeap() / 1024) + " KB";
  doc["bHeap"] = String(bBoardHeap) + " KB";
  doc["on"] = maaOn; 
  doc["wsConnected"] = wsConnected;
  doc["bBoardOnline"] = bBoardOnline;
  doc["activeUser"] = (activeUserIndex >= 0) ? users[activeUserIndex].username : "无";
  String u = "";
  for (int i = 0; i < USER_COUNT; i++) { u += "• " + users[i].username + (users[i].isAdmin ? " [管理员]" : "") + "\n"; }
  doc["users"] = u;
  doc["logs"] = logMsg1 + "\n" + logMsg2 + "\n" + logMsg3;
  String out; serializeJson(doc, out); sendJson(200, out);
}

void handleStaticFile(const String& path, const String& contentType) {
  if (!LittleFS.exists(path)) { server.send(404, "text/plain", "File not found"); return; }
  File file = LittleFS.open(path, "r"); server.streamFile(file, contentType); file.close();
}

void setup() {
  Serial.begin(115200); 
  toB.begin(38400);     
  
  if (!LittleFS.begin()) { LittleFS.format(); LittleFS.begin(); }
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_POST, handleLoginPost);
  server.on("/logout", HTTP_GET, handleLogout);
  server.on("/maa/state", HTTP_GET, handleMaaState);
  server.on("/maa/set", HTTP_GET, handleMaaSet);
  server.on("/admin/data", HTTP_GET, handleAdminData);
  server.on("/avatar.jpg", HTTP_GET, []() { handleStaticFile("/avatar.jpg", "image/jpeg"); });
  server.on("/avatar.png", HTTP_GET, []() { handleStaticFile("/avatar.png", "image/png"); });
  
  server.begin();
  addLog("A板启动完成");
}

void loop() {
  server.handleClient();
  processSerial(); 

  if (bBoardOnline && millis() - lastBPingTime > 10000) {
    bBoardOnline = false;
    addLog("副控板心跳超时，已离线");
  }
}
