#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SoftwareSerial.h>

// 软串口：GPIO12(D6) 为 RX，GPIO14(D5) 为 TX
SoftwareSerial fromA(12, 14); 

// ============================================================
// =================== ⚙️ 配置区 ===================
// ============================================================
const char* WIFI_SSID     = "enpty";
const char* WIFI_PASSWORD = "ZC20051215";

const char* MAA_HTTP_IP   = "192.168.31.169";  
const int   MAA_HTTP_PORT = 8848;              
const char* MAA_WS_PATH   = "/API/V1";         

const char* ADB_PATH      = "D:/MAA-v6.16.8-win-x64/adb/platform-tools/adb.exe"; 
const char* ADB_HOST      = "127.0.0.1:16384"; 
// ============================================================

WebSocketsClient webSocket;
String currentMaaConnectId = ""; 

int maaActionState = 0;
int connectRetry = 0; 
unsigned long lastActionTime = 0;
String tasksParam = ""; 
String stageParam = "";
String seriesParam = "0"; 
int medParam = 0, stoneParam = 0;
String timesParam = "2147483647";

unsigned long lastMaaMsgTime = 0;
String serialBuffer = "";
unsigned long lastPingTime = 0;

// ============================================================
// ★ 终极全量翻译字典（v5 完整整合版）
//    - v2 增强：Fight/基建/信用/领奖/连接/抄作业 细节
//    - v3 补充：公招细节 / NotEnoughStaff / 资源识别 / MuMu 等
//    - v4 补充：ResolutionChanged / RoutingRestart
//    - 官方文档补全：ParadoxCopilot / Debug / ExceededLimit /
//      肉鸽无前缀枚举 / Stage*Enter / StartGameTask / StageInfoError 等
//    去重：ResolutionGot 只保留一处
// ============================================================
String translateMaaText(String text) {
  if (text == "" || text == "null") return "";

  // ============================================================
  // 一、TaskChain（任务链）
  // ============================================================
  if (text == "StartUp") return "开始唤醒";
  if (text == "CloseDown") return "关闭游戏";
  if (text == "Fight") return "刷理智";
  if (text == "Mall") return "信用收支";
  if (text == "Recruit") return "自动公招";
  if (text == "Infrast") return "基建换班";
  if (text == "Award") return "领取奖励";
  if (text == "Roguelike") return "肉鸽探索";
  if (text == "Copilot") return "自动抄作业";
  if (text == "SSSCopilot") return "保全作业";
  if (text == "ParadoxCopilot") return "自动抄悖论模拟作业";
  if (text == "Depot") return "仓库识别";
  if (text == "OperBox") return "干员识别";
  if (text == "Reclamation") return "生息演算";
  if (text == "Custom") return "自定义任务";
  if (text == "SingleStep") return "单步任务";
  if (text == "VideoRecognition") return "视频识别";
  if (text == "Debug") return "调试";

  // ============================================================
  // 二、具体动作 (task)
  // ============================================================
  if (text == "StartButton1") return "点击开始";
  if (text == "StartButton2") return "点击开始(代理)";
  if (text == "MedicineConfirm") return "使用理智药";
  if (text == "ExpiringMedicineConfirm") return "使用将过期理智药";
  if (text == "StoneConfirm") return "碎石恢复理智";
  if (text == "RecruitRefreshConfirm") return "刷新公招标签";
  if (text == "RecruitConfirm") return "确认招募";
  if (text == "RecruitNowConfirm") return "使用加急许可";
  if (text == "ReportToPenguinStats") return "汇报企鹅物流";
  if (text == "ReportToYituliu") return "汇报一图流";
  if (text == "InfrastDormDoubleConfirmButton") return "基建宿舍二次确认";

  // ============================================================
  // 三、Fight 战斗细节
  // ============================================================
  if (text == "BattleStart") return "战斗开始";
  if (text == "BattleStartAll") return "全体战斗开始";
  if (text == "BattleOperDetail") return "查看干员详情";
  if (text == "BattleFormation") return "编队";
  if (text == "BattleFormationSelected") return "选定编队";
  if (text == "UseMedicine") return "使用理智药";
  if (text == "UseStone") return "使用源石";

  // ============================================================
  // 四、状态 (what / subtask)
  // ============================================================
  if (text == "ScreencapCost") return "计算截图耗时";
  if (text == "FastestWayToScreencap") return "选择最优截图";
  if (text == "ResolutionInfo") return "分辨率信息";
  if (text == "EmulatorFPS") return "模拟器帧率";
  if (text == "SanityBeforeStage") return "进图前理智不足";
  if (text == "FightTimes") return "次数/倍率异常";
  if (text == "StageInfo") return "关卡信息";
  if (text == "StageInfoError") return "关卡识别错误";
  if (text == "UnsupportedLevel") return "不支持的关卡";
  if (text == "StartGameTask") return "启动游戏失败";
  if (text == "SwitchTheme@ToggleSettingsMenu") return "切换主题";
  if (text == "EnterFacility") return "进入设施";
  if (text == "ProductOfFacility") return "收取产物";
  if (text == "ClickIt") return "点击操作";
  if (text == "SwipeToTheLeft") return "向左滑动";
  if (text == "SwipeToTheRight") return "向右滑动";
  if (text == "SwipeToTheUp") return "向上滑动";
  if (text == "SwipeToTheDown") return "向下滑动";
  if (text == "WaitLoading") return "等待加载";
  if (text == "DoNothing") return "待机";
  if (text == "ExceededLimit") return "任务执行次数达到上限";
  if (text == "RoutingRestart") return "肉鸽路由重开（主动放弃）";

  // ============================================================
  // 五、公招与掉落
  // ============================================================
  if (text == "StageDrops") return "关卡掉落";
  if (text == "RecruitTagsDetected") return "识别到公招标签";
  if (text == "RecruitSpecialTag") return "识别到高级标签";
  if (text == "RecruitRobotTag") return "识别到机械标签";
  if (text == "RecruitResult") return "公招结果";
  if (text == "RecruitRefresh") return "刷新公招";
  if (text == "RecruitContinue") return "公招继续";
  if (text == "RecruitPreservedTag") return "识别到保底标签";
  if (text == "RecruitTagsRefreshed") return "公招标签已刷新";
  if (text == "RecruitNoPermit") return "无招聘许可，跳过";
  if (text == "RecruitPermitReserved") return "招聘许可触发保底跳过";
  if (text == "RecruitPermitCountRecognitionFailed") return "招聘许可数量识别失败";
  if (text == "RecruitTagsSelected") return "已选择公招标签";
  if (text == "RecruitError") return "公招识别错误";

  // ============================================================
  // 六、基建与日常
  // ============================================================
  if (text == "InfrastStart") return "基建开始";
  if (text == "InfrastEnter") return "进入基建";
  if (text == "MallStart") return "信用收支开始";
  if (text == "AwardStart") return "领取奖励开始";
  if (text == "VisitFriends") return "访问好友";
  if (text == "MallShopping") return "信用商店购物";
  if (text == "MallBuy") return "购买物品";
  if (text == "SwitchFacility") return "切换设施";
  if (text == "Dormitory") return "进入宿舍";
  if (text == "Training") return "进入训练室";
  if (text == "TrainingStart") return "开始训练";
  if (text == "Reception") return "进入会客室";
  if (text == "Office") return "进入办公室";
  if (text == "Processing") return "进入加工站";
  if (text == "Power") return "进入发电站";
  if (text == "Trade") return "进入贸易站";
  if (text == "Mfg") return "进入制造站";
  if (text == "Control") return "进入控制中枢";
  if (text == "NotEnoughStaff") return "干员不足，跳过设施";

  // ============================================================
  // 七、领奖
  // ============================================================
  if (text == "MailStart") return "领取邮件";
  if (text == "OrundumStart") return "领取合成玉";
  if (text == "FreeGacha") return "免费抽卡";
  if (text == "SignInEvent") return "活动签到";

  // ============================================================
  // 八、抄作业
  // ============================================================
  if (text == "CopilotAction") return "作业：执行操作";
  if (text == "CopilotComplete") return "作业：执行完毕";
  if (text == "CopilotListLoad") return "作业列表加载中";
  if (text == "CopilotListLoadError") return "作业列表加载失败";
  if (text == "CopilotTaskStart") return "作业任务开始";
  if (text == "CopilotTaskSuccess") return "作业任务成功";
  if (text == "CopilotTaskFail") return "作业任务失败";

  // ============================================================
  // 九、资源识别
  // ============================================================
  if (text == "DepotInfo") return "仓库识别结果";
  if (text == "OperBoxInfo") return "干员识别结果";

  // ============================================================
  // 十、连接底层状态
  // ============================================================
  if (text == "Connected") return "已连接模拟器";
  if (text == "UuidGot") return "获取设备UUID";
  if (text == "ResolutionGot") return "获取分辨率";
  if (text == "ResolutionChanged") return "分辨率已变更";
  if (text == "ResolutionDetected") return "分辨率已检测";
  if (text == "UnsupportedResolution") return "分辨率不支持";
  if (text == "ResolutionError") return "分辨率错误";
  if (text == "Reconnecting") return "断开，重连中";
  if (text == "Reconnected") return "重连成功";
  if (text == "Disconnect") return "连接断开";
  if (text == "ScreencapFailed") return "截图失败";
  if (text == "TouchModeNotAvailable") return "触控模式不支持";
  if (text == "ConnectFailed") return "连接失败";
  if (text == "InitFailed") return "初始化失败";
  if (text == "InternalError") return "内部错误";
  if (text == "Timeout") return "连接超时";
  if (text == "DeviceNotFound") return "未找到设备";
  if (text == "AdbError") return "ADB 错误";
  if (text == "GameStart") return "游戏已启动";
  if (text == "GameStop") return "游戏已关闭";
  if (text == "MuMuExtrasInputStatus") return "MuMu 触控增强状态";
  if (text == "AsyncCallInfo") return "异步调用信息";

  // ============================================================
  // 十一、肉鸽 (Roguelike) —— 带 Roguelike1 前缀
  // ============================================================
  if (text == "StartExplore") return "开始肉鸽探索";
  if (text == "StageTraderInvestConfirm") return "肉鸽投资源石锭";
  if (text == "StageTraderInvestSystemFull") return "肉鸽投资上限";
  if (text == "ExitThenAbandon") return "放弃本次肉鸽";
  if (text == "Roguelike1MissionCompletedFlag") return "肉鸽：任务完成";
  if (text == "Roguelike1MissionFailedFlag") return "肉鸽：任务失败";
  if (text == "Roguelike1StageSafeHouseEnter") return "肉鸽：进入安全屋";
  if (text == "Roguelike1StageEncounterEnter") return "肉鸽：遭遇事件";
  if (text == "Roguelike1StageCambatDpsEnter") return "肉鸽：进入作战";
  if (text == "Roguelike1StageEmergencyDps") return "肉鸽：紧急作战";
  if (text == "Roguelike1StageDreadfulFoe") return "肉鸽：险路恶敌";
  if (text == "RoguelikeSelectDifficulty") return "肉鸽：选择难度";
  if (text == "RoguelikeSelectSquad") return "肉鸽：选择分队";
  if (text == "RoguelikeSelectRoles") return "肉鸽：选择职业";
  if (text == "RoguelikeStartGame") return "肉鸽：开局";
  if (text == "RoguelikeStageRecruit") return "肉鸽：招募干员";
  if (text == "RoguelikeStageBattle") return "肉鸽：进入作战";
  if (text == "RoguelikeStageEvent") return "肉鸽：遭遇事件";
  if (text == "RoguelikeStageShop") return "肉鸽：进入商店";
  if (text == "RoguelikeStageTreasure") return "肉鸽：开启宝箱";
  if (text == "RoguelikeSettlement") return "肉鸽：结算";

  // ============================================================
  // 十二、肉鸽 —— 不带 Roguelike1 前缀（官方文档枚举）
  // ============================================================
  if (text == "MissionCompletedFlag") return "肉鸽战斗完成";
  if (text == "MissionFailedFlag") return "肉鸽战斗失败";
  if (text == "StageTraderEnter") return "肉鸽关卡：诡异行商";
  if (text == "StageSafeHouseEnter") return "肉鸽关卡：安全屋";
  if (text == "StageEncounterEnter") return "肉鸽关卡：不期而遇/古堡馈赠";
  if (text == "StageCombatOpsEnter") return "肉鸽关卡：普通作战";
  if (text == "StageEmergencyOps") return "肉鸽关卡：紧急作战";
  if (text == "StageDreadfulFoe") return "肉鸽关卡：险路恶敌";

  return text; 
}

// ============================================================
// ★ 2. 解析所有类型的回调
// ============================================================
String parseMaaCallback(JsonObject dataObj) {
  int msgId = dataObj["msg"] | 0;
  JsonVariant details = dataObj["details"];
  String logText = "";
  String rawText = "";

  // 全局信息
  if (msgId == 0) return "❌ 内部错误 (InternalError)";
  if (msgId == 1) return "❌ 初始化失败 (InitFailed)";
  if (msgId == 2) { 
    if (details.is<JsonObject>()) rawText = details["what"] | "";
    if (rawText != "" && rawText != "null") return translateMaaText(rawText);
    return "";
  }
  if (msgId == 3) { 
    String taskchain = translateMaaText(details["taskchain"] | "");
    fromA.println("STATE:0"); 
    return "✔ 全部任务结束 (" + taskchain + ")";
  }
  if (msgId == 4) return "🔗 异步调用信息"; 
  if (msgId == 5) return "⏹ 实例已销毁 (Destroyed)"; 

  // 任务链
  if (msgId >= 10000 && msgId <= 10004) {
    String taskchain = translateMaaText(details["taskchain"] | ""); 
    if (msgId == 10000) return "▶ 开始: " + taskchain;
    if (msgId == 10001) return "✔ 完成: " + taskchain; 
    if (msgId == 10002) return "⚠ 中断: " + taskchain; 
    if (msgId == 10003) return "⏹ 停止: " + taskchain; 
    if (msgId == 10004) return "ℹ 额外信息: " + taskchain;
  }
  
  // 子任务
  if (msgId >= 20000 && msgId <= 20004) {
    String what = details["what"] | "";
    String why = details["why"] | "";
    String taskchain = translateMaaText(details["taskchain"] | "");
    
    if (what == "" || what == "DoNothing" || what == "null") return "";

    String transWhat = translateMaaText(what);
    String transWhy = translateMaaText(why);

    if (msgId == 20000) return "  ↳ [" + taskchain + "] 开始: " + transWhat;
    if (msgId == 20001) return "  ↳ [" + taskchain + "] 执行: " + transWhat;
    if (msgId == 20002) return ""; 
    if (msgId == 20003) {
      if (why != "" && why != "null") return "  ❌ 异常: " + transWhat + " (" + transWhy + ")";
      return "  ❌ 异常: " + transWhat;
    }
    if (msgId == 20004) return "  ⏹ 停止: " + transWhat;
  }

  // Web 请求
  if (msgId == 30000) return "🌐 发起 Web 请求";

  return "";
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      fromA.println("WS:0");
      fromA.println("SYS:B板与MAA连接断开！");
      break;
    case WStype_CONNECTED:
      fromA.println("WS:1");
      fromA.println("SYS:B板已成功连接MAA服务器！"); 
      webSocket.sendTXT("{\"command\":\"callBack\",\"msgId\":0,\"data\":{}}");
      break;
    case WStype_TEXT: {
      String msg = String((char*)payload);
      DynamicJsonDocument doc(3072); 
      if (!deserializeJson(doc, msg)) {
        lastMaaMsgTime = millis(); 
        JsonObject dataObj = doc["data"].as<JsonObject>();
        if (!dataObj.isNull()) {
          String logText = parseMaaCallback(dataObj);
          if (logText != "") fromA.println("LOG:" + logText); 
        }
      }
      break;
    }
    default: break;
  }
}

String sendMaaPost(String path, String jsonBody) {
  WiFiClient client;
  HTTPClient http;
  String url = "http://" + String(MAA_HTTP_IP) + ":" + String(MAA_HTTP_PORT) + path;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(3000); 
  int httpCode = http.POST(jsonBody);
  String response = "";
  if (httpCode == 200) response = http.getString();
  http.end();
  return response;
}

void processSerial() {
  while (fromA.available()) {
    char c = fromA.read();
    if (c == '\n') {
      serialBuffer.trim();
      if (serialBuffer.startsWith("START:")) {
        String data = serialBuffer.substring(6);
        
        // ★ 解析 6 个参数（增加了代理倍率 series）
        int idx1 = data.indexOf('|');
        int idx2 = data.indexOf('|', idx1 + 1);
        int idx3 = data.indexOf('|', idx2 + 1);
        int idx4 = data.indexOf('|', idx3 + 1);
        int idx5 = data.indexOf('|', idx4 + 1);
        
        if (idx1 > 0 && idx2 > 0 && idx3 > 0 && idx4 > 0 && idx5 > 0) {
          tasksParam = data.substring(0, idx1);
          stageParam = data.substring(idx1 + 1, idx2);
          seriesParam = data.substring(idx2 + 1, idx3);
          medParam = data.substring(idx3 + 1, idx4).toInt();
          stoneParam = data.substring(idx4 + 1, idx5).toInt();
          timesParam = data.substring(idx5 + 1);
          
          maaActionState = 1; 
          connectRetry = 0;
          lastActionTime = millis();
          lastMaaMsgTime = millis();
          fromA.println("STATE:1");
          fromA.println("SYS:收到指令，正在连接MAA...");
        }
      } else if (serialBuffer == "STOP") {
        maaActionState = 0;
        if (currentMaaConnectId != "") {
          sendMaaPost("/API/V1/stop", "{\"id\":\"" + currentMaaConnectId + "\"}");
          currentMaaConnectId = "";
        }
        fromA.println("STATE:0");
        fromA.println("SYS:已停止任务并断开连接");
      }
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }
}

void setup() {
  Serial.begin(115200); 
  fromA.begin(38400);   
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  webSocket.begin(MAA_HTTP_IP, MAA_HTTP_PORT, MAA_WS_PATH);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(10000);
  webSocket.enableHeartbeat(15000, 3000, 2); 
  
  fromA.println("SYS:B板已启动，正在连接MAA..."); 
}

void loop() {
  webSocket.loop();
  processSerial();

  if (millis() - lastPingTime > 3000) {
    lastPingTime = millis();
    int freeHeap = ESP.getFreeHeap() / 1024; 
    fromA.println("PING:" + String(freeHeap));
  }

  if (maaActionState > 0 && millis() - lastActionTime > 500) {
    lastActionTime = millis();
    
    if (maaActionState == 1) {
      String connectResp = sendMaaPost("/API/V1/connect", "{\"adbPath\":\"" + String(ADB_PATH) + "\",\"host\":\"" + String(ADB_HOST) + "\",\"detailJson\":\"\"}");
      int idIndex = connectResp.indexOf("\"id\":\"");
      if (idIndex >= 0) {
        int endIndex = connectResp.indexOf("\"", idIndex + 6);
        if (endIndex > idIndex) currentMaaConnectId = connectResp.substring(idIndex + 6, endIndex);
      }
      if (currentMaaConnectId != "") {
        maaActionState = 2; 
        fromA.println("SYS:模拟器已连接");
      } else {
        connectRetry++;
        if (connectRetry >= 3) {
          fromA.println("SYS:MAA连接失败");
          fromA.println("STATE:0");
          maaActionState = 0;
        }
      }
    } 
    else if (maaActionState == 2) {
      if (tasksParam.indexOf("StartUp") >= 0) sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"StartUp\",\"params\":{\"client_type\":\"Official\",\"start_game_enabled\":true}}");
      maaActionState = 3;
    }
    else if (maaActionState == 3) {
      if (tasksParam.indexOf("Fight") >= 0) {
        String fightJson = "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Fight\",\"params\":{\"stage\":\"" + stageParam + "\",\"medicine\":" + String(medParam) + ",\"stone\":" + String(stoneParam) + ",\"times\":" + timesParam + ",\"series\":" + seriesParam + "}}";
        sendMaaPost("/API/V1/appendTask", fightJson);
      }
      maaActionState = 4;
    }
    else if (maaActionState == 4) {
      if (tasksParam.indexOf("Infrast") >= 0) sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Infrast\",\"params\":{\"mode\":20000,\"facility\":[\"Mfg\",\"Trade\",\"Control\",\"Power\",\"Reception\",\"Office\",\"Dorm\",\"Processing\",\"Training\"],\"drones\":\"Money\"}}");
      maaActionState = 5;
    }
    else if (maaActionState == 5) {
      if (tasksParam.indexOf("Recruit") >= 0) sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Recruit\",\"params\":{\"select\":[4],\"confirm\":[3,4],\"times\":4,\"refresh\":true,\"force_refresh\":true}}");
      maaActionState = 6;
    }
    else if (maaActionState == 6) {
      if (tasksParam.indexOf("Mall") >= 0) sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Mall\",\"params\":{\"shopping\":true,\"buy_first\":[\"招聘许可\"],\"blacklist\":[\"碳\",\"家具\",\"加急许可\"],\"visit_friends\":true}}");
      maaActionState = 7;
    }
    else if (maaActionState == 7) {
      if (tasksParam.indexOf("Award") >= 0) sendMaaPost("/API/V1/appendTask", "{\"id\":\"" + currentMaaConnectId + "\",\"type\":\"Award\",\"params\":{\"award\":true,\"mail\":true,\"free_gacha\":true,\"orundum\":true,\"mining\":true,\"specialaccess\":true,\"sign_in_event\":false}}");
      maaActionState = 8;
    }
    else if (maaActionState == 8) {
      sendMaaPost("/API/V1/start", "{\"id\":\"" + currentMaaConnectId + "\"}");
      fromA.println("SYS:所有任务已送达");
      maaActionState = 0; 
    }
  }
  
  if (maaActionState == 0 && millis() - lastMaaMsgTime > 300000 && currentMaaConnectId != "") {
    fromA.println("STATE:0");
    currentMaaConnectId = "";
  }
}