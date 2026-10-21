const char PAGE_HTML[] PROGMEM = R"HTMLPAGE(
<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no" />
<title>Enpty - MAA 控制台</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:linear-gradient(-45deg,#a8c0ff,#d5a6f0,#ffb8d1,#b8a0ff);background-size:400% 400%;animation:gradientBG 20s ease infinite;color:#2a1f3d;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI","PingFang SC","Microsoft YaHei",sans-serif;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:2em 1em;overflow-x:hidden;overflow-y:auto;position:relative;}
@keyframes gradientBG{0%{background-position:0% 50%}50%{background-position:100% 50%}100%{background-position:0% 50%}}
#bgLayer{position:fixed;inset:0;overflow:hidden;pointer-events:none;z-index:0}
.blob{position:absolute;border-radius:50%;filter:blur(70px);opacity:.55;will-change:transform}
.blob1{width:420px;height:420px;background:radial-gradient(circle,#ffb8d1,#d5a6f0);top:-12%;left:-10%;animation:blobFloat1 24s ease-in-out infinite}
.blob2{width:380px;height:380px;background:radial-gradient(circle,#a8c0ff,#b8a0ff);bottom:-15%;right:-10%;animation:blobFloat2 28s ease-in-out infinite}
.blob3{width:320px;height:320px;background:radial-gradient(circle,#ffe0f0,#ffb8d1);top:45%;right:12%;animation:blobFloat3 32s ease-in-out infinite}
.blob4{width:300px;height:300px;background:radial-gradient(circle,#c5d8ff,#a8c0ff);bottom:18%;left:8%;animation:blobFloat4 26s ease-in-out infinite}
@keyframes blobFloat1{0%,100%{transform:translate(0,0) scale(1)}33%{transform:translate(70px,50px) scale(1.15)}66%{transform:translate(30px,110px) scale(.9)}}
@keyframes blobFloat2{0%,100%{transform:translate(0,0) scale(1)}50%{transform:translate(-90px,-50px) scale(1.15)}}
@keyframes blobFloat3{0%,100%{transform:translate(0,0) scale(1)}50%{transform:translate(-70px,70px) scale(.85)}}
@keyframes blobFloat4{0%,100%{transform:translate(0,0) scale(1)}50%{transform:translate(80px,-40px) scale(1.1)}}
.petal{position:absolute;top:-30px;width:12px;height:12px;background:radial-gradient(circle at 30% 30%,#ffd1e8,#ff9ec7 70%);border-radius:50% 0 50% 0;box-shadow:0 0 6px rgba(255,158,199,.5);animation:petalFall linear infinite;opacity:.85;will-change:transform;pointer-events:none}
@keyframes petalFall{0%{transform:translateY(-30px) translateX(0) rotate(0deg);opacity:0}10%{opacity:.85}90%{opacity:.85}100%{transform:translateY(110vh) translateX(var(--drift,60px)) rotate(720deg);opacity:0}}
.star{position:absolute;background:radial-gradient(circle,#fff 20%,rgba(255,255,255,0) 70%);border-radius:50%;pointer-events:none;animation:starTwinkle 3s ease-in-out infinite}
@keyframes starTwinkle{0%,100%{opacity:.15;transform:scale(.5)}50%{opacity:1;transform:scale(1.3)}}
.loader{position:fixed;inset:0;background:linear-gradient(-45deg,#a8c0ff,#d5a6f0,#ffb8d1,#b8a0ff);background-size:400% 400%;animation:gradientBG 20s ease infinite;display:flex;align-items:center;justify-content:center;flex-direction:column;z-index:9999;transition:opacity .8s ease,visibility .8s}
.loader.hide{opacity:0;visibility:hidden;pointer-events:none}
.loader-wrap{position:relative;width:100px;height:150px;margin-bottom:24px}
.lemon-emoji{position:absolute;top:0;left:50%;transform:translateX(-50%);font-size:2.6em;line-height:1;animation:lemonSqueeze 1.4s ease-in-out infinite;z-index:2;filter:drop-shadow(0 4px 10px rgba(0,0,0,.15))}
@keyframes lemonSqueeze{0%,100%{transform:translateX(-50%) rotate(-14deg)}25%{transform:translateX(-50%) rotate(12deg)}50%{transform:translateX(-50%) rotate(-14deg)}75%{transform:translateX(-50%) rotate(12deg)}}
.cup{position:absolute;bottom:0;left:50%;transform:translateX(-50%);width:80px;height:96px;border:2px solid rgba(255,255,255,.9);border-top:none;border-radius:6px 6px 34px 34px;background:linear-gradient(180deg,rgba(255,255,255,.3),rgba(255,255,255,.08));backdrop-filter:blur(8px);-webkit-backdrop-filter:blur(8px);overflow:hidden;box-shadow:inset 0 -8px 20px rgba(0,0,0,.08),inset 2px 0 6px rgba(255,255,255,.4),inset -2px 0 6px rgba(255,255,255,.4),0 12px 32px rgba(168,192,255,.4)}
.cup::before{content:'';position:absolute;top:0;left:8px;width:6px;height:100%;background:linear-gradient(180deg,rgba(255,255,255,.65),transparent 75%);border-radius:3px;pointer-events:none;z-index:3}
.liquid{position:absolute;bottom:0;left:0;right:0;height:0%;background:linear-gradient(180deg,#ffe066,#f7c948,#f0b400);border-radius:0 0 32px 32px;animation:liquidFill 1.4s ease-in-out infinite;box-shadow:0 -6px 16px rgba(247,201,72,.55)}
.liquid::before{content:'';position:absolute;top:-5px;left:0;right:0;height:10px;background:radial-gradient(ellipse at center,rgba(255,255,255,.7) 0%,rgba(255,224,102,.4) 55%,transparent 85%);border-radius:50%}
@keyframes liquidFill{0%{height:0%}60%{height:74%}85%{height:74%}100%{height:0%}}
.loader-text{color:#fff;font-size:1.1em;letter-spacing:.35em;margin-bottom:.6em;text-shadow:0 2px 10px rgba(168,192,255,.5)}
.loader-sub{color:rgba(255,255,255,.75);font-size:.9em;letter-spacing:.45em}
#wrapper{width:100%;max-width:29em;position:relative;z-index:2;margin:auto;margin-top:20px;margin-bottom:20px;}
#main{position:relative;background:linear-gradient(135deg,rgba(255,245,252,.6) 0%,rgba(255,255,255,.42) 45%,rgba(245,240,255,.55) 100%);backdrop-filter:blur(40px) saturate(180%) brightness(1.05);-webkit-backdrop-filter:blur(40px) saturate(180%) brightness(1.05);border:2px solid rgba(255,255,255,.85);border-radius:28px;padding:3.5em 2.5em 3em;text-align:center;box-shadow:0 24px 60px rgba(120,90,180,.28),0 8px 20px rgba(120,90,180,.15),inset 0 2px 4px rgba(255,255,255,1),inset 0 -2px 4px rgba(255,255,255,.5),inset 2px 0 4px rgba(255,255,255,.8),inset -2px 0 4px rgba(255,255,255,.8);transition:transform 1s cubic-bezier(.4,0,.2,1);overflow:hidden}
#main::before{content:'';position:absolute;top:0;left:8%;right:8%;height:35%;background:radial-gradient(ellipse at top,rgba(255,255,255,.85),transparent 70%);border-radius:28px 28px 50% 50%;pointer-events:none;z-index:1}
#main::after{content:'';position:absolute;bottom:0;left:0;right:0;height:35%;background:linear-gradient(0deg,rgba(168,192,255,.15),transparent);border-radius:0 0 28px 28px;pointer-events:none;z-index:1}
.corner-deco{position:absolute;font-size:16px;color:rgba(168,192,255,.6);z-index:3;pointer-events:none;user-select:none;text-shadow:0 0 8px rgba(168,192,255,.8)}
.corner-deco.tl{top:12px;left:16px}.corner-deco.tr{top:12px;right:16px}.corner-deco.bl{bottom:12px;left:16px}.corner-deco.br{bottom:12px;right:16px}
#main>header{transition:opacity .5s ease;position:relative;z-index:2}.hide-v{display:none!important}
.conn-group{position:absolute;top:1em;right:1.2em;display:flex;flex-direction:column;gap:.4em;z-index:4}
.conn{display:flex;align-items:center;justify-content:flex-end;gap:.35em;font-size:.68em;color:rgba(42,31,61,.6);letter-spacing:.1em;font-weight:600}
.conn .dot{width:7px;height:7px;border-radius:50%;background:#888;transition:background .3s}
.conn.online .dot{background:#10b981;box-shadow:0 0 8px rgba(16,185,129,.7)}
.conn.offline .dot{background:#ef4444;box-shadow:0 0 8px rgba(239,68,68,.7)}
.avatar{width:100px;height:100px;margin:0 auto 1em;border-radius:50%;overflow:hidden;box-shadow:0 10px 26px rgba(168,192,255,.5),inset 0 2px 8px rgba(255,255,255,.5);transition:transform .3s;background:linear-gradient(135deg,#a8c0ff,#ffb8d1);display:block;border:3px solid rgba(255,255,255,.95);position:relative}
.avatar:hover{transform:scale(1.05) rotate(-3deg)}
.avatar img{width:100%;height:100%;object-fit:cover;display:block}
h1{font-size:1.6em;font-weight:700;letter-spacing:.12em;margin-bottom:.5em;color:#2a1f3d;font-family:"Segoe UI",sans-serif}
p{color:rgba(42,31,61,.72);line-height:1.8;font-size:.9em;margin-bottom:.8em}
.hint{font-size:.78em;color:rgba(42,31,61,.55);letter-spacing:.1em}
.button{display:inline-block;padding:.8em 1.4em;border-radius:12px;border:1.5px solid rgba(255,255,255,.9);background:rgba(255,255,255,.55);color:#2a1f3d;font-size:.85em;cursor:pointer;transition:all .3s;letter-spacing:.1em;text-decoration:none;font-family:inherit;font-weight:600;position:relative;overflow:hidden;backdrop-filter:blur(12px) saturate(150%);-webkit-backdrop-filter:blur(12px) saturate(150%);box-shadow:0 4px 12px rgba(120,90,180,.12),inset 0 1.5px 0 rgba(255,255,255,1),inset 0 -1px 0 rgba(255,255,255,.4)}
.button:hover{background:rgba(255,255,255,.8);transform:translateY(-2px);box-shadow:0 8px 20px rgba(120,90,180,.22),inset 0 1.5px 0 rgba(255,255,255,1)}
.button.fit{display:block;width:100%;margin:.8em 0 .4em 0;}
.button.small{padding:.5em 1.1em;font-size:.7em;border-radius:10px}
.button.on{background:linear-gradient(135deg,#a8c0ff,#d5a6f0);border:1.5px solid rgba(255,255,255,.6);color:#fff;font-weight:700;box-shadow:0 6px 20px rgba(168,192,255,.5),inset 0 1.5px 0 rgba(255,255,255,.6)}
.button.off{background:linear-gradient(135deg,#ffb8d1,#d5a6f0);border:1.5px solid rgba(255,255,255,.6);color:#fff;font-weight:700;box-shadow:0 6px 20px rgba(255,184,209,.5),inset 0 1.5px 0 rgba(255,255,255,.6)}
.button.on:hover,.button.off:hover{transform:translateY(-3px) scale(1.02)}
.button:active{transform:translateY(0) scale(.98);opacity:.9}
.button:disabled{opacity:.4;cursor:not-allowed;transform:none!important}
hr{border:0;border-top:1px solid rgba(255,255,255,.8);margin:1.4em 0}
.input{width:100%;padding:1em 1.2em;margin-bottom:1em;border-radius:12px;border:1.5px solid rgba(255,255,255,.9);background:rgba(255,255,255,.6);color:#2a1f3d;font-size:.9em;font-family:inherit;letter-spacing:.05em;outline:none;transition:all .3s;backdrop-filter:blur(12px);-webkit-backdrop-filter:blur(12px);box-shadow:inset 0 1.5px 0 rgba(255,255,255,1),inset 0 -1px 0 rgba(255,255,255,.4)}
.input:focus{border-color:rgba(213,166,240,.9);background:rgba(255,255,255,.85);box-shadow:0 0 0 4px rgba(213,166,240,.2),inset 0 1.5px 0 rgba(255,255,255,1)}
.input::placeholder{color:rgba(42,31,61,.4)}
select.input { appearance: none; -webkit-appearance: none; background-image: url("data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%22292.4%22%20height%3D%22292.4%22%3E%3Cpath%20fill%3D%22%232a1f3d%22%20d%3D%22M287%2069.4a17.6%2017.6%200%200%200-13-5.4H18.4c-5%200-9.3%201.8-12.9%205.4A17.6%2017.6%200%200%200%200%2082.2c0%205%201.8%209.3%205.4%2012.9l128%20127.9c3.6%203.6%207.8%205.4%2012.8%205.4s9.2-1.8%2012.8-5.4L287%2095c3.5-3.5%205.4-7.8%205.4-12.8%200-5-1.9-9.2-5.5-12.8z%22%2F%3E%3C%2Fsvg%3E"); background-repeat: no-repeat; background-position: right .7em top 50%; background-size: .65em auto; padding-right: 2em; }
.status{padding:.85em;border-radius:12px;margin:.9em 0;font-size:.85em;letter-spacing:.12em;font-weight:600;transition:all .4s;position:relative;backdrop-filter:blur(10px);-webkit-backdrop-filter:blur(10px)}
.status.on{background:rgba(16,185,129,.2);color:#0a7a52;border:1.5px solid rgba(16,185,129,.5);animation:breathe 2s ease-in-out infinite}
.status.off{background:rgba(239,68,68,.14);color:#b91c3e;border:1.5px solid rgba(239,68,68,.4)}
@keyframes breathe{0%,100%{box-shadow:0 0 0 rgba(16,185,129,0)}50%{box-shadow:0 0 24px rgba(16,185,129,.4)}}
.task-grid{display:grid;grid-template-columns:1fr 1fr;gap:.6em;margin-top:.5em;}
.task-item{display:flex;align-items:center;background:rgba(255,255,255,.7);padding:.6em;border-radius:8px;font-size:.75em;font-weight:600;color:#2a1f3d;cursor:pointer;border:1.5px solid transparent;transition:all .2s;user-select:none;}
.task-item input{margin-right:.5em;accent-color:#8a5fc8;transform:scale(1.1);}
.task-item:has(input:checked){background:rgba(213,166,240,.3);border-color:rgba(213,166,240,.8);}
.log{text-align:left;padding:.85em;border-radius:12px;background:rgba(255,255,255,.6);border:1.5px solid rgba(255,255,255,.9);font-family:"SF Mono",Menlo,Consolas,monospace;font-size:.7em;line-height:1.8;color:rgba(42,31,61,.8);white-space:pre-wrap;word-break:break-all;min-height:80px;max-height:160px;overflow-y:auto;margin:.7em 0;backdrop-filter:blur(10px);-webkit-backdrop-filter:blur(10px);box-shadow:inset 0 1.5px 0 rgba(255,255,255,1),inset 0 -1px 0 rgba(255,255,255,.4)}
.log::-webkit-scrollbar{width:4px}
.log::-webkit-scrollbar-thumb{background:rgba(213,166,240,.5);border-radius:2px}
.log .log-start { color: #3b82f6; font-weight: 600; }
.log .log-done { color: #10b981; font-weight: 600; }
.log .log-error { color: #ef4444; font-weight: 600; }
.log .log-drop { color: #f59e0b; font-weight: 600; }
.id-card{padding:.85em;border-radius:12px;margin-bottom:.8em;text-align:left;background:rgba(213,166,240,.18);border:1.5px solid rgba(255,255,255,.8);backdrop-filter:blur(12px);transition:all .3s;box-shadow:inset 0 1.5px 0 rgba(255,255,255,.9),inset 0 -1px 0 rgba(255,255,255,.4)}
.id-card:hover{background:rgba(213,166,240,.3);border-color:rgba(255,255,255,1)}
.id-card .label{font-size:.7em;color:#8a5fc8;letter-spacing:.12em;margin-bottom:.4em;font-weight:700}
#footer{text-align:center;margin-top:1.5em}
.copyright{list-style:none}
.copyright li{font-size:.68em;color:rgba(255,255,255,.9);letter-spacing:.1em;line-height:2;text-shadow:0 1px 4px rgba(120,90,180,.3)}
.copyright a{color:rgba(255,255,255,.9);text-decoration:none}
.copyright a:hover{color:#fff}
.swap-anim{background-image:linear-gradient(-45deg,#2a1f3d 40%,#a86fd8 50%,#2a1f3d 60%);background-clip:text;-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-size:300%;animation:swap 4s linear infinite;display:inline-block}
@keyframes swap{0%{background-position:300%}50%{background-position:0%}100%{background-position:300%}}
.toast{position:fixed;bottom:2em;left:50%;transform:translateX(-50%) translateY(120px);background:linear-gradient(135deg,#a8c0ff,#d5a6f0);color:#fff;padding:.7em 1.5em;border-radius:14px;font-size:.8em;font-weight:600;letter-spacing:.1em;transition:transform .4s,opacity .4s;z-index:99999;max-width:90vw;text-align:center;opacity:0;pointer-events:none;box-shadow:0 8px 24px rgba(168,192,255,.5),inset 0 1.5px 0 rgba(255,255,255,.6)}
.toast.show{transform:translateX(-50%) translateY(0);opacity:1}
.ws-log{border-color:rgba(168,192,255,.8); background:rgba(240,248,255,.6);}
.ws-log .label{color:#4a5fc8;}
.content-container {display: flex;flex-direction: column;gap: 1em;text-align: left;}
.col-left, .col-right {display: flex;flex-direction: column;gap: 1em;}
@media (min-width: 768px) {
  body { align-items: flex-start; padding-top: 3em; }
  #wrapper { max-width: 62em; }
  #main { padding: 3em 3em 2em; min-height: 75vh; display: flex; flex-direction: column; }
  .content-container {display: grid;grid-template-columns: 1.1fr 1fr;gap: 1.5em;align-items: stretch;flex: 1;}
  .col-left, .col-right { height: 100%; }
  .col-right .id-card { flex: 1; display: flex; flex-direction: column; }
  .col-right .log { flex: 1; max-height: none !important; }
  .actions-area { margin-top: auto; display: flex; flex-direction: column; }
  .id-card { margin-bottom: 0; }
}
@media (max-width:480px){
  #main{padding:2.5em 1.5em 2em;border-radius:22px}
  h1{font-size:1.4em}
  .button{font-size:.8em;padding:.7em 1.2em}
  .blob{filter:blur(40px)}
}
</style>
</head>
<body class="is-preload">
<div id="bgLayer"><div class="blob blob1"></div><div class="blob blob2"></div><div class="blob blob3"></div><div class="blob blob4"></div></div>
<div class="loader" id="loader">
  <div class="loader-wrap"><div class="lemon-emoji">🍋</div><div class="cup"><div class="liquid"></div></div></div>
  <p class="loader-text">榨汁</p><p class="loader-sub">再喝</p>
</div>
<div id="wrapper">
  <section id="main">
    <span class="corner-deco tl">✿</span><span class="corner-deco tr">✦</span><span class="corner-deco bl">✦</span><span class="corner-deco br">✿</span>
    <header id="loginView" class="hide-v">
      <h1 class="swap-anim">MAA 控制台</h1>
      <p class="hint" style="margin-bottom:2em">请输入用户名和密码</p>
      <input type="text" class="input" id="username" placeholder="用户名" autocomplete="username" />
      <input type="password" class="input" id="password" placeholder="密码" autocomplete="current-password" />
      <button class="button fit on" onclick="doLogin()" id="loginBtn">登 录</button>
    </header>
    <header id="maaView" class="hide-v">
      <div class="conn-group">
        <div class="conn" id="connStateA"><span class="dot"></span><span id="connTextA">主控板连接中</span></div>
        <div class="conn" id="connStateB"><span class="dot"></span><span id="connTextB">副控板连接中</span></div>
      </div>
      <span class="avatar"><img src="/avatar.jpg" alt="avatar" onerror="this.style.display='none';this.parentNode.textContent='E';this.parentNode.style.lineHeight='100px';this.parentNode.style.fontSize='2.5em';this.parentNode.style.display='flex';this.parentNode.style.alignItems='center';this.parentNode.style.justifyContent='center'" /></span>
      <h1>MAA 控制台</h1>
      <p class="hint" id="welcomeUser" style="margin-bottom:1.5em"></p>
      <div class="content-container">
        <div class="col-left">
          <div class="id-card" style="background:rgba(255,255,255,.4);border-color:rgba(255,255,255,1)">
            <div class="label">⚙️ 任务选择（打勾即执行）</div>
            <div class="task-grid">
              <label class="task-item"><input type="checkbox" id="task_StartUp" checked> 开始唤醒</label>
              <label class="task-item"><input type="checkbox" id="task_Fight" checked> 理智作战</label>
              <label class="task-item"><input type="checkbox" id="task_Recruit" checked> 自动公招</label>
              <label class="task-item"><input type="checkbox" id="task_Infrast" checked> 基建换班</label>
              <label class="task-item"><input type="checkbox" id="task_Mall" checked> 信用收支</label>
              <label class="task-item"><input type="checkbox" id="task_Award" checked> 领取奖励</label>
            </div>
          </div>
          
          <div class="id-card" style="background:rgba(255,255,255,.4);border-color:rgba(255,255,255,1)">
            <div class="label">⚔️ 理智作战参数</div>
            <div style="display:flex; gap:.5em; margin-bottom:.5em;">
              <input type="text" class="input" id="fight_stage" placeholder="关卡 (如 1-7)" value="1-7" style="margin:0; padding:.6em; font-size:.8em; flex:2;">
              <select class="input" id="fight_series" style="margin:0; padding:.6em; font-size:.8em; flex:1;">
                <option value="0">AUTO</option>
                <option value="10">10</option>
                <option value="9">9</option>
                <option value="8">8</option>
                <option value="7">7</option>
                <option value="6">6</option>
                <option value="5">5</option>
                <option value="4">4</option>
                <option value="3">3</option>
                <option value="2">2</option>
                <option value="1">1</option>
                <option value="-1">不切换</option>
              </select>
            </div>
            <div style="display:flex; gap:.5em; align-items:center;">
              <span style="font-size:.75em; color:#8a5fc8; white-space:nowrap;">吃药:</span>
              <input type="number" class="input" id="fight_med" value="0" min="0" style="margin:0; padding:.6em; font-size:.8em; flex:1;">
              <span style="font-size:.75em; color:#8a5fc8; white-space:nowrap;">碎石:</span>
              <input type="number" class="input" id="fight_stone" value="0" min="0" style="margin:0; padding:.6em; font-size:.8em; flex:1;">
              <span style="font-size:.75em; color:#8a5fc8; white-space:nowrap;">次数:</span>
              <input type="number" class="input" id="fight_times" value="2147483647" min="1" style="margin:0; padding:.6em; font-size:.8em; flex:1;">
            </div>
          </div>

          <div class="actions-area">
            <div id="status" class="status off">● 状态加载中...</div>
            <button class="button fit on" onclick="setState('on', this)" id="btnOn">▶ 开启选定任务</button>
            <button class="button fit off" onclick="setState('off', this)" id="btnOff">■ 停止当前任务</button>
          </div>
        </div>
        <div class="col-right">
          <div class="id-card" style="background:rgba(240,240,240,.5); border-color:rgba(255,255,255,1)">
            <div class="label" style="color:#666;">⚙️ 系统运行日志</div>
            <div class="log" id="log" style="margin:0; border:none; background:transparent; padding:0;">加载中...</div>
          </div>
          <div class="id-card ws-log">
            <div class="label">🚀 MAA 实时数据源 <span id="wsDot" style="color:#ef4444;">●</span></div>
            <div class="log" id="wsLog" style="margin:0; border:none; background:transparent; padding:0;">等待 MAA 连接...</div>
          </div>
        </div>
      </div>
      <hr />
      <div id="adminLink" class="hide-v" style="margin-bottom:.6em">
        <button class="button small fit" onclick="toAdmin()">⚙ 后台管理</button>
      </div>
      <button class="button small fit" onclick="doLogout()">退出登录</button>
    </header>
    <header id="adminView" class="hide-v">
      <h1>⚙ 后台</h1>
      <p class="hint" style="margin-bottom:1.2em">系统信息与用户状态</p>
      <div class="log" id="adminInfo" style="text-align:left; min-height:300px;">加载中...</div>
      <hr />
      <button class="button small fit" onclick="backToMaa()">返回控制台</button>
      <button class="button small fit" onclick="doLogout()">退出登录</button>
    </header>
  </section>
  <footer id="footer"><ul class="copyright"><li></li><li><a href="https://beian.miit.gov.cn/" target="_blank">沪ICP备2024049069号-1</a></li></ul></footer>
</div>
<div class="toast" id="toast"></div>
<script>
setTimeout(function() { var loader = document.getElementById('loader'); if (loader) loader.classList.add('hide'); document.body.classList.remove('is-preload'); }, 1500);
(function initBackground() {
  var bg = document.getElementById('bgLayer');
  for (var i = 0; i < 16; i++) { (function() { var p = document.createElement('div'); p.className = 'petal'; var size = 8 + Math.random() * 8; p.style.width = size + 'px'; p.style.height = size + 'px'; p.style.left = Math.random() * 100 + '%'; p.style.animationDuration = (8 + Math.random() * 10) + 's'; p.style.animationDelay = (Math.random() * 12) + 's'; p.style.setProperty('--drift', ((Math.random() - 0.5) * 200) + 'px'); if (Math.random() > 0.6) p.style.opacity = 0.55; bg.appendChild(p); })(); }
  for (var j = 0; j < 30; j++) { var s = document.createElement('div'); s.className = 'star'; var sw = Math.random() * 4 + 2; s.style.width = sw + 'px'; s.style.height = sw + 'px'; s.style.left = Math.random() * 100 + '%'; s.style.top = Math.random() * 100 + '%'; s.style.animationDelay = (Math.random() * 3) + 's'; s.style.animationDuration = (2 + Math.random() * 3) + 's'; bg.appendChild(s); }
})();
function getToken() { return localStorage.getItem('maa_token') || ''; }
function setToken(t) { localStorage.setItem('maa_token', t); }
function clearToken() { localStorage.removeItem('maa_token'); }
function getUsername() { return localStorage.getItem('maa_username') || ''; }
function setUsername(u) { localStorage.setItem('maa_username', u); }
var connFailCount = 0;
function setConnStateA(online) { var el = document.getElementById('connStateA'); var txt = document.getElementById('connTextA'); if(!el || !txt) return; if (online) { el.className = 'conn online'; txt.textContent = '主控板已连接'; } else { el.className = 'conn offline'; txt.textContent = '主控板离线'; } }
function setConnStateB(online) { var el = document.getElementById('connStateB'); var txt = document.getElementById('connTextB'); if(!el || !txt) return; if (online) { el.className = 'conn online'; txt.textContent = '副控板已连接'; } else { el.className = 'conn offline'; txt.textContent = '副控板离线'; } }
function showView(viewId) { var ids = ['loginView', 'maaView', 'adminView']; for (var i = 0; i < ids.length; i++) { var el = document.getElementById(ids[i]); if (el) { if (ids[i] === viewId) el.classList.remove('hide-v'); else el.classList.add('hide-v'); } } }
var currentDeg = 0;
function rotateCard(d) { var ele = document.getElementById('main'); if (!ele) return; currentDeg += d; ele.style.transition = 'transform 1s cubic-bezier(.4,0,.2,1)'; ele.style.transform = 'rotateY(' + currentDeg + 'deg)'; setTimeout(function() { if (Math.abs(currentDeg) >= 360) { ele.style.transition = 'none'; ele.style.transform = 'rotateY(0deg)'; currentDeg = 0; setTimeout(function() { ele.style.transition = 'transform 1s cubic-bezier(.4,0,.2,1)'; }, 50); } }, 1000); }
var adminTimer = null;
function toAdmin() { showView('adminView'); rotateCard(-360); loadAdmin(); if (adminTimer) clearInterval(adminTimer); adminTimer = setInterval(loadAdmin, 5000); }
function backToMaa() { if (adminTimer) { clearInterval(adminTimer); adminTimer = null; } showView('maaView'); rotateCard(360); }
function toast(msg) { var t = document.getElementById('toast'); if(!t) return; t.textContent = msg; if (t._hideTimer) { clearTimeout(t._hideTimer); t._hideTimer = null; } void t.offsetWidth; t.classList.add('show'); t._hideTimer = setTimeout(function() { t.classList.remove('show'); t._hideTimer = null; }, 2000); }
function doLogin() {
  var u = document.getElementById('username').value.trim(); var p = document.getElementById('password').value;
  if (!u || !p) { toast('请输入用户名和密码'); return; }
  var btn = document.getElementById('loginBtn'); btn.disabled = true; btn.textContent = '登录中...';
  fetch('/login', { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: 'username=' + encodeURIComponent(u) + '&password=' + encodeURIComponent(p) })
    .then(function(r) { if (r.status === 200) { return r.text().then(function(token) { token = token.trim(); if (!token) { toast('登录失败：服务器未返回 token'); return; } setToken(token); setUsername(u); showView('maaView'); rotateCard(-360); initControl(u); toast('登录成功'); }); } else { toast('登录失败：用户名或密码错误'); } })
    .catch(function(err) { toast('网络错误: ' + err); }).finally(function() { btn.disabled = false; btn.textContent = '登 录'; });
}
function doLogout() { var token = getToken(); fetch('/logout?token=' + encodeURIComponent(token)).finally(function() { clearToken(); if (stateTimer) { clearInterval(stateTimer); stateTimer = null; } if (adminTimer) { clearInterval(adminTimer); adminTimer = null; } document.getElementById('username').value = ''; document.getElementById('password').value = ''; showView('loginView'); rotateCard(360); }); }
var stateTimer = null;
function initControl(username) { document.getElementById('welcomeUser').textContent = '👤 ' + username; loadState(); if (stateTimer) clearInterval(stateTimer); stateTimer = setInterval(loadState, 5000); }
function formatLogHtml(text) { if (!text) return ''; return text.split('\n').map(function(line) { var cls = ''; if (line.indexOf('▶') >= 0) cls = 'log-start'; else if (line.indexOf('✔') >= 0) cls = 'log-done'; else if (line.indexOf('❌') >= 0 || line.indexOf('⚠') >= 0) cls = 'log-error'; else if (line.indexOf('🎁') >= 0) cls = 'log-drop'; var safeLine = line.replace(/</g, '&lt;').replace(/>/g, '&gt;'); if (cls) return '<span class="' + cls + '">' + safeLine + '</span>'; return safeLine; }).join('\n'); }
function loadState() {
  var mv = document.getElementById('maaView'); if (mv.classList.contains('hide-v')) return; var token = getToken(); if (!token) { showView('loginView'); return; }
  var controller = new AbortController(); var timeoutId = setTimeout(function() { controller.abort(); }, 15000);
  fetch('/maa/state?token=' + encodeURIComponent(token), { signal: controller.signal })
    .then(function(r) { clearTimeout(timeoutId); if (r.status === 401) { clearToken(); if (stateTimer) { clearInterval(stateTimer); stateTimer = null; } showView('loginView'); toast('会话已过期，请重新登录'); return null; } return r.json(); })
    .then(function(d) {
      if (!d) return; connFailCount = 0; setConnStateA(true); setConnStateB(d.bBoardOnline);
      var el = document.getElementById('status'); if (d.pending && d.activeUserIndex !== d.myIndex) { el.className = 'status on'; el.textContent = '⏳ ' + d.activeUser + ' 正在控制，等待释放...'; } else if (d.on) { el.className = 'status on'; if (d.actionState == 1) el.textContent = '⏳ 正在连接模拟器...'; else if (d.actionState > 1 && d.actionState < 8) el.textContent = '⏳ 正在下发任务...'; else el.textContent = '⏳ 任务执行中...'; } else { el.className = 'status off'; el.textContent = '○ 已待命'; }
      if (d.username) document.getElementById('welcomeUser').textContent = '👤 ' + d.username; if (d.isAdmin) document.getElementById('adminLink').classList.remove('hide-v'); else document.getElementById('adminLink').classList.add('hide-v');
      var wsd = document.getElementById('wsDot'); if (wsd) { wsd.style.color = d.wsConnected ? '#10b981' : '#ef4444'; }
      var lg = document.getElementById('log'); var txt = ''; if (d.log1) txt += d.log1 + '\n'; if (d.log2) txt += d.log2 + '\n'; if (d.log3) txt += d.log3 + '\n'; var newText = txt || '暂无日志'; var formattedHtml = formatLogHtml(newText); if (lg.innerHTML !== formattedHtml) { var atBottom = lg.scrollTop + lg.clientHeight >= lg.scrollHeight - 20; lg.innerHTML = formattedHtml; if (atBottom) lg.scrollTop = lg.scrollHeight; }
      var wlg = document.getElementById('wsLog'); var wtxt = ''; if (d.wsLog1) wtxt += d.wsLog1 + '\n'; if (d.wsLog2) wtxt += d.wsLog2 + '\n'; if (d.wsLog3) wtxt += d.wsLog3 + '\n'; var wNewText = wtxt || '等待 MAA 连接...'; var wFormattedHtml = formatLogHtml(wNewText); if (wlg.innerHTML !== wFormattedHtml) { var wAtBottom = wlg.scrollTop + wlg.clientHeight >= wlg.scrollHeight - 20; wlg.innerHTML = wFormattedHtml; if (wAtBottom) wlg.scrollTop = wlg.scrollHeight; }
    })
    .catch(function(err) { clearTimeout(timeoutId); connFailCount++; if (connFailCount >= 3) { setConnStateA(false); setConnStateB(false); var el = document.getElementById('status'); if (el) { el.className = 'status off'; el.textContent = '● 网络无响应'; } } });
}
function setState(s, btn) {
  var token = getToken(); if (!token) { showView('loginView'); return; } var origText = btn.textContent; btn.disabled = true; btn.textContent = '下发中...';
  var tasks = []; if (document.getElementById('task_StartUp').checked) tasks.push('StartUp'); if (document.getElementById('task_Fight').checked) tasks.push('Fight'); if (document.getElementById('task_Recruit').checked) tasks.push('Recruit'); if (document.getElementById('task_Infrast').checked) tasks.push('Infrast'); if (document.getElementById('task_Mall').checked) tasks.push('Mall'); if (document.getElementById('task_Award').checked) tasks.push('Award'); var taskStr = tasks.join(',');
  
  var stage = document.getElementById('fight_stage').value.trim() || '1-7'; 
  var series = document.getElementById('fight_series').value || '0'; 
  var med = document.getElementById('fight_med').value || '0'; 
  var stone = document.getElementById('fight_stone').value || '0'; 
  var times = document.getElementById('fight_times').value || '2147483647';

  fetch('/maa/set?state=' + s + '&tasks=' + encodeURIComponent(taskStr) + '&stage=' + encodeURIComponent(stage) + '&series=' + series + '&med=' + med + '&stone=' + stone + '&times=' + times + '&token=' + encodeURIComponent(token))
    .then(function(r) { if (r.status === 401) { clearToken(); if (stateTimer) { clearInterval(stateTimer); stateTimer = null; } showView('loginView'); return null; } if (r.status === 409 || r.status === 429) return r.text().then(function(t) { toast(t); return null; }); return r.text(); })
    .then(function(t) { if (t !== null) { toast(s === 'on' ? '已下发：开启' : '已下发：停止'); loadState(); } })
    .catch(function(e) { toast('操作失败: ' + e); }).finally(function() { btn.disabled = false; btn.textContent = origText; });
}
function loadAdmin() {
  var token = getToken(); fetch('/admin/data?token=' + encodeURIComponent(token)).then(function(r) { return r.json(); }).then(function(d) {
    var txt = '';
    txt += '📡 设备信息\n';
    txt += '  A板内存   ' + d.aHeap + '\n'; 
    txt += '  B板内存   ' + d.bHeap + '\n'; 
    txt += '  WS 状态   ' + (d.wsConnected ? '已连接' : '未连接') + '\n';
    txt += '  B板状态   ' + (d.bBoardOnline ? '已连接' : '离线') + '\n\n';
    txt += '👥 用户列表\n' + d.users + '\n\n';
    txt += '🎮 MAA 状态\n';
    txt += '  当前控制者  ' + d.activeUser + '\n';
    txt += '  开关状态   ' + (d.on ? '已开启' : '已关闭') + '\n\n';
    txt += '📋 最近日志\n' + d.logs;
    document.getElementById('adminInfo').textContent = txt;
  }).catch(function() { document.getElementById('adminInfo').textContent = '加载失败，请返回重试'; });
}
function checkLogin() {
  var token = getToken(); var uname = getUsername(); if (!token) { showView('loginView'); return; }
  fetch('/maa/state?token=' + encodeURIComponent(token)).then(function(r) {
    if (r.status === 200) {
      return r.json().then(function(d) {
        showView('maaView'); setConnStateA(true); setConnStateB(d.bBoardOnline);
        if (d.username) { document.getElementById('welcomeUser').textContent = '👤 ' + d.username; setUsername(d.username); } else if (uname) { document.getElementById('welcomeUser').textContent = '👤 ' + uname; }
        if (d.isAdmin) document.getElementById('adminLink').classList.remove('hide-v'); else document.getElementById('adminLink').classList.add('hide-v');
        loadState(); if (stateTimer) clearInterval(stateTimer); stateTimer = setInterval(loadState, 5000);
      });
    } else { clearToken(); showView('loginView'); }
  }).catch(function() { showView('loginView'); });
}
window.addEventListener('load', function() { checkLogin(); });
</script>
</body>
</html>
)HTMLPAGE";