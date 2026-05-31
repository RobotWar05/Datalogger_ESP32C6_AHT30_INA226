#include "WifiConfigPage.h"

const char wifi_config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="vi">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <title>WIFI CONFIG</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600;700;800&display=swap" rel="stylesheet">
  <style>
    :root { --bg-color: #dbeafe; --card-bg: #f0f9ff; --text-main: #1e293b; --text-sub: #475569; --accent-color: #2563eb; --shadow-soft: 0 4px 20px -5px rgba(30, 58, 138, 0.15); --focus-ring: 0 0 0 3px rgba(37, 99, 235, 0.3); }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: 'Montserrat', sans-serif; background-color: var(--bg-color); color: #0f172a; line-height: 1.6; font-weight: 400; -webkit-font-smoothing: antialiased; display: flex; flex-direction: column; align-items: center; min-height: 100vh; padding: 30px; }
    .container { width: 100%; max-width: 400px; display: flex; flex-direction: column; gap: 20px; }
    .brand { text-align: center; font-weight: 800; font-size: 1.5rem; color: #1e3a8a; text-transform: uppercase; margin-bottom: 10px; letter-spacing: 1px; }
    .card { background: var(--card-bg); border-radius: 24px; padding: 30px; box-shadow: var(--shadow-soft); border: 1px solid #bfdbfe; }
    .form-group { margin-bottom: 20px; }
    .form-group label { display: block; margin-bottom: 8px; color: #334155; font-weight: 600; font-size: 0.9rem; text-transform: uppercase; }
    input { width: 100%; padding: 12px; border-radius: 10px; border: 1px solid #cbd5e1; background: #ffffff; color: #0f172a; font-family: 'Montserrat', sans-serif; font-size: 1rem; outline: none; transition: border 0.3s; }
    input:focus { border-color: var(--accent-color); box-shadow: var(--focus-ring); }
    .btn { width: 100%; padding: 14px; border: none; border-radius: 10px; cursor: pointer; font-weight: 700; font-family: 'Montserrat', sans-serif; font-size: 1rem; transition: 0.2s; margin-bottom: 10px; text-transform: uppercase; }
    .btn:focus { outline: none; box-shadow: var(--focus-ring); }
    .btn:active { transform: scale(0.98); }
    .btn-primary { background: #2563eb; color: white; }
    .btn-primary:hover { background: #1d4ed8; }
    .btn-secondary { background: #e0f2fe; color: #0284c7; }
    .btn-secondary:hover { background: #bae6fd; }
    .wifi-list { margin-top: 20px; max-height: 200px; overflow-y: auto; border-top: 1px solid #e2e8f0; padding-top: 10px; }
    .wifi-item { padding: 10px; border-radius: 8px; cursor: pointer; display: flex; justify-content: space-between; align-items: center; transition: 0.2s; color: var(--text-main); font-weight: 600; }
    .wifi-item:hover { background: #dbeafe; color: #1e40af; }
    .signal-icon { width: 20px; height: 20px; fill: var(--text-sub); }
    .loader { border: 3px solid #f3f3f3; border-radius: 50%; border-top: 3px solid #3498db; width: 20px; height: 20px; animation: spin 1s linear infinite; display: inline-block; margin-right: 10px; vertical-align: middle; display: none; }
    @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
    footer { margin-top: 30px; font-size: 0.8rem; color: var(--text-sub); opacity: 0.8; text-align: center; font-weight: 600; }
  </style>
</head>
<body>
  <div class="container">
    <div class="brand">WIFI CONFIG</div>
    <div class="card">
      <div class="form-group"><label>SSID (Tên Wifi)</label><input type="text" id="ssid" placeholder="Đang tìm mạng..."></div>
      <div class="form-group"><label>Password</label><input type="password" id="pass" placeholder="Nhập mật khẩu..."></div>
      <button class="btn btn-primary" onclick="saveConfig()">LƯU & KẾT NỐI</button>
      <button class="btn btn-secondary" onclick="scanWifi()"><div class="loader" id="scanLoader"></div> QUÉT LẠI</button>
      <div id="wifiList" class="wifi-list"><div style="text-align: center; color: #64748b; font-size: 0.9rem; padding: 10px;">Đang quét mạng...</div></div>
    </div>
  </div>
  <footer></footer>
  <script>
    function scanWifi() {
      const listDiv = document.getElementById('wifiList');
      const loader = document.getElementById('scanLoader');
      listDiv.innerHTML = '';
      loader.style.display = 'inline-block';
      fetch('/scan').then(res => res.json()).then(data => {
        loader.style.display = 'none';
        listDiv.innerHTML = ''; 
        if(data.length === 0) { listDiv.innerHTML = '<div style="text-align:center; padding:10px;">Không tìm thấy mạng nào</div>'; return; }
        data.forEach(ssid => {
          const item = document.createElement('div');
          item.className = 'wifi-item';
          item.innerHTML = `<span>${ssid}</span><svg class="signal-icon" viewBox="0 0 24 24"><path d="M12.01 21.49L23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l11.63 14.49.01.01.01-.01z"/></svg>`;
          item.onclick = function() { document.getElementById('ssid').value = ssid; document.getElementById('pass').focus(); };
          listDiv.appendChild(item);
        });
        document.getElementById('ssid').placeholder = "Chọn hoặc nhập tên Wifi";
      }).catch(err => { loader.style.display = 'none'; listDiv.innerHTML = '<div style="color:red; text-align:center;">Lỗi quét mạng!</div>'; });
    }
    function saveConfig() {
      const ssid = document.getElementById('ssid').value;
      const pass = document.getElementById('pass').value;
      
      if(!ssid) { alert("Vui lòng nhập tên Wifi!"); return; }
      if(pass.length > 0 && pass.length < 8) { alert("Mật khẩu phải từ 8 ký tự trở lên!"); return; }
      
      const btn = document.querySelector('.btn-primary');
      const originalText = btn.innerText;
      btn.innerText = "ĐANG LƯU...";
      btn.disabled = true;
      
      fetch('/save', { 
          method: 'POST', 
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, 
          body: 'ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass)
      }).then(res => {  
          if(res.ok) alert("Đã lưu! Thiết bị sẽ khởi động lại."); 
          else { alert("Lỗi khi lưu!"); btn.innerText = originalText; btn.disabled = false; }
      }).catch(err => alert("Đã lưu. ESP32 đang khởi động lại..."));
    }
    window.onload = scanWifi;
  </script>
</body>
</html>
)rawliteral";
