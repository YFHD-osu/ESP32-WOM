const MQTT_BROKER = "__MQTT_BROKER__";
const MQTT_PORT = "__MQTT_PORT__";

class RwdChecker {
  static isMobileDevice() {
    const userAgent = navigator.userAgent.toLowerCase();
    // 檢查 User-Agent 字串是否包含 mobile 或其他手機相關關鍵字
    return /mobile|android|iphone|ipad|ipod|blackberry|opera mini|iemobile/.test(userAgent);
  }

  static setDisplayMode() {
    if (RwdChecker.isMobileDevice()) {
      document.getElementById("login-window").classList.add('container_static');
      document.getElementById("device-list").classList.add('container_static');
    }
  }
}

class Client {
  constructor() {
    this.boker = `wss://${MQTT_BROKER}:${MQTT_PORT}/mqtt`;
    this.inSession = false;
    
    this.deviceCount = 0;
    this.numberOfDevice = 0;

    this.deviceListCache = "";

    this.refreshTimer = null;
  }

  setLoginSpin(start) {
    const submitBtn = document.getElementById('submitBtn');
    const submitIcon = submitBtn.querySelector('i');
    const loadingSpinner = document.getElementById('login-loading');

    if (start) {
      submitIcon.style.display = 'none';
      loadingSpinner.style.display = 'flex';
    } else {
      submitIcon.style.display = 'flex';
      loadingSpinner.style.display = 'none';
    }
    
  }

  setRefreshSpin(start) {
    const button = document.getElementById('refresh')
    const text = document.getElementById('refresh-text');
    const spinner = document.getElementById('refresh-loading');

    if (start) {
      button.disabled = true;
      text.style.display = 'none';
      spinner.style.display = 'flex';
    } else {
      button.disabled = false;
      text.style.display = 'block';
      spinner.style.display = 'none';
    }
    
  }

  showLogin() {
    const login = document.getElementById('login-window');
    const device = document.getElementById('device-list');
    
    login.style.display = "flex";
    device.style.display = "none";
  }
  
  showDevices() {
    const login = document.getElementById('login-window');
    const device = document.getElementById('device-list');
    
    login.style.display = "none";
    device.style.display = "flex";
  }

  login(username, password) {
    if (this.client != null) {
      this.logout();
    }

    this.setLoginSpin(true);

    this.client = mqtt.connect(this.boker, {
      username: username,
      password: password,
      clientId: "web-client-" + Math.random().toString(16).substr(2, 8),
    });

    this.client.on('error', (err) => {
      console.log('MQTT error:', err);

      this.client.end(true, () => { });

      if (String(err).includes("Not authorized") || String(err).includes("Bad username or password")) {
        alert("不正確的使用者帳號或密碼");
      } else {
        alert(err);
      }

      this.setLoginSpin(false);
      clearInterval(this.refreshTimer);
    });

    this.client.on("connect", () => {
      console.log("✅ Connected (Browser)");
      this.client.subscribe("esp32/wake");
      this.refresh();
      this.setLoginSpin(false);

      this.inSession = true;

      // Clean password field
      document.getElementById("password-field").value = "";
      
      this.showDevices();

      this.refreshTimer = setInterval(() => {
        this.client.publish("esp32/wake", "{\"cmd\": \"query\"}");
        this.setRefreshSpin(true);
      }, 10000);

    });

    this.client.on("message", (topic, message) => {

      if (this.numberOfDevice > 0) {
        console.log(`${topic}: AAA${message.toString()}`);

        const data = JSON.parse(message);
        
        let node = this._createDeviceCard(data);
        this.deviceListCache += node.innerHTML;
        
        this.numberOfDevice = this.numberOfDevice-1;
        
        // Clean up on last device added
        if (this.numberOfDevice == 0) {
          var viewPort = document.querySelector("#list-viewport");
          viewPort.innerHTML = this.deviceListCache;
          this.setRefreshSpin(false);
        }

        return;
      }

      // If a number received, means the device list transmitting began
      if (Number(message) != NaN) {
        console.log(`${topic}: BBB${message.toString()}`);
        this.numberOfDevice = Number(message);
        this.deviceCount = Number(message);
        
        this.deviceListCache = "";
        return;
      }

      console.log(`${topic}: ${message.toString()}`);
    });
  }

  logout() {
    this.client.end(true, this.onClientEnd);
    this.showLogin();

    clearInterval(this.refreshTimer);
  }

  refresh() {
    this.client.publish("esp32/wake", "{\"cmd\": \"query\"}");

    this.setRefreshSpin(true);
  }

  wakeDevice(id) {
    this.client.publish("esp32/wake", `{\"cmd\": \"wake\",\"dev\": ${id}}`);
  }

  _createDeviceCard(json) {
    var element = document.createElement('div');
    var isOnline = parseInt(json.st) == 1;
    var color = isOnline ? "#68bd5a" : "#f36356";

    let id = this.deviceCount - this.numberOfDevice;
    
    element.innerHTML = `
    <button id="status-dev-${id}" onclick="client.wakeDevice(${id})">
      <img src="${json.im}" alt="Icon of ${json.na}">
      <span id="name"> ${json.na} </span>
      <span id="ip"> ${json.ip} </span>
      <span id="status" style="background-color: ${color}">
    </button>`;

    return element
  }

}

RwdChecker.setDisplayMode();
let client = new Client();

function setShadow(el) {
  const isScrollable = el.scrollHeight > el.clientHeight;

  // GUARD: If element is not scrollable, remove all classes
  if (!isScrollable) {
    el.classList.remove('is-bottom-overflowing', 'is-top-overflowing');
    return;
  }

  // Otherwise, the element is overflowing!
  // Now we just need to find out which direction it is overflowing to (can be both).
  // One pixel is added to the height to account for non-integer heights.
  const isScrolledToBottom = el.scrollHeight < el.clientHeight + el.scrollTop + 1;
  const isScrolledToTop = isScrolledToBottom ? false : el.scrollTop === 0;
  el.classList.toggle('is-bottom-overflowing', !isScrolledToBottom);
  el.classList.toggle('is-top-overflowing', !isScrolledToTop);
}

document.getElementById("login-form").addEventListener("submit", function (e) {
  e.preventDefault(); // 阻止表單真的送出 & 重新整理頁面

  const username = document.getElementById("username-field").value;
  const password = document.getElementById("password-field").value;

  client.login(username, password); // 呼叫你的函式
});

document.querySelector('.list-wrapper')
  .addEventListener('scroll', (e) => {
    const el = e.currentTarget;
    setShadow(el);
  });