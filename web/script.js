const MQTT_BROKER = "__MQTT_BROKER__";
const MQTT_PORT = "__MQTT_PORT__";
const MQTT_BROKER_WS =  "wss://n7429497.ala.us-east-1.emqxsl.com:8084/mqtt";

const HTML_SPINNER = `
<div class="ispinner">
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
  <div class="ispinner-blade"></div>
</div>
`;

class DeviceCard {
    static CreateDeviceCard(name, description, icon, online) {
        var element = document.createElement('div');

        element.innerHTML = `
        <li class="list-item">
            <div class="list-item-content">
                <i class="${icon}"></i>
                <div class="list-item-text">
                    <h3>${name}</h3>
                    <p>${description}</p>
                </div>
                <div class="list-item-spinner">
                    ${HTML_SPINNER}
                    <div class="status-indicator" data-online="${online}"></div>
                </div>
            </div>
        </li>
        `;

        return element;
    }

    static ClearDeviceCard() {
        const deviceList = document.getElementById('device-list');
        deviceList.innerHTML = "";
    }

    static GetDeviceCardCount() {
        const deviceList = document.getElementById('device-list');
        return deviceList.children.length;
    }

    static FindDeviceCardIndex(name) {
        const deviceList = document.getElementById('device-list');
        var i = 0;

        for (const child of deviceList.children) {
            const titleElement = child.querySelector('h3');
            
            if (titleElement && titleElement.textContent.trim() === name) {
                return i;
            }
                
            i++;
        }

        return null;
    }

    static FindDeviceCard(name) {
        const deviceList = document.getElementById('device-list');

        var result = DeviceCard.FindDeviceCardIndex(name);

        
        if (result == null) {
            console.log("HEHEHE")
            return null;
        }
        
        return deviceList.children[result];
    }
};

class ElementControl {
    static SetLoginStatus(isLogining) {
        const button = document.querySelector('.btn-signin-inline');

        if (isLogining) {
            button.innerHTML = HTML_SPINNER;
        } else {
            button.innerHTML = '<i class="bi bi-arrow-right-circle-fill fs-3"></i>';
        }
    }

    static SetLoginPage() {
        const loginForm = document.getElementById('loginForm');
        const listContainer = document.getElementById('listContainer');

        loginForm.classList.remove('hidden');
        loginForm.classList.add('active');
        listContainer.classList.remove('active');
        listContainer.classList.add('hidden');
    }

    static SetDevicePage() {
        const loginForm = document.getElementById('loginForm');
        const listContainer = document.getElementById('listContainer');

        loginForm.classList.remove('active');
        loginForm.classList.add('hidden');
        listContainer.classList.remove('hidden');
        listContainer.classList.add('active');
    }

    static SetListLoading(isLoading) {
        const loadingContainer = document.getElementById('device-list-loading');

        if (isLoading) {
            loadingContainer.classList.remove('hidden');
        } else {
            loadingContainer.classList.add('hidden');
        }
    }

    static UpdateCardLoading(card, isLoading) {
        const spinner = card.querySelector('.list-item-spinner');
        
        if (isLoading) {
            spinner.classList.add('active');
        } else {
            spinner.classList.remove('active');
        }
    }

    static UpdateCardOnline(card, online) {
        const indicator = card.querySelector('.status-indicator');
        
        indicator.setAttribute('data-online', online);

    }
};

class Client {
  constructor() {
    this.client = null;
    this.deviceCount = 0;
  }

  login(username, password) {
    ElementControl.SetLoginStatus(true);

    this.client = mqtt.connect(MQTT_BROKER_WS, {
        username: username,
        password: password,
        clientId: "web-client-" + Math.random().toString(16).substr(2, 8),
    });

    this.client.on('error', this.onErrorCallback.bind(this));
    this.client.on('message', this.onMessageCallback.bind(this));
    this.client.on('connect', this.onConnectCallback.bind(this));    
  }

  logout() {
    if (this.client) {
        this.client.end( true, () => {});
    }

    if (this.refreshTimer) {
        clearInterval(this.refreshTimer);
    }

    this.client = null;

    DeviceCard.ClearDeviceCard();
    ElementControl.SetLoginPage();
  }

  onMessageCallback(topic, message) {
    if (message == "OK") {
        return;
    }

    // If revieved a number, then set the device count
    if (! isNaN(Number(message))) {
        console.log(`${topic}: Got device count number: ${message.toString()}`);
        this.deviceCount = parseInt(message);

        if (DeviceCard.GetDeviceCardCount() != this.deviceCount) {
            DeviceCard.ClearDeviceCard();
            ElementControl.SetListLoading(true);
        }

        return;
    }

    console.log(`${topic}: Got device data pack: ${message.toString()}`);

    const data = JSON.parse(message);

    var name = data.na;
    var description = data.ip;
    var icon = data.im;
    var online = data.st == "1";

    if (name == null || description == null || icon == null || online == null) {
        console.log("Invalid data pack recieved");
        return;
    }

    // Still filling the list
    if (DeviceCard.GetDeviceCardCount() < this.deviceCount) {
        const deviceList = document.getElementById('device-list');

        var newCard = DeviceCard.CreateDeviceCard(name, description, icon, online);

        var deviceID = DeviceCard.GetDeviceCardCount();
        newCard.addEventListener('click', function() {
            this.client.publish("esp32/wake", `{\"cmd\": \"wake\",\"dev\": ${deviceID}}`);
            ElementControl.UpdateCardLoading(newCard, true);
            console.log(`Waking deivce with id ${deviceID}`)
        }.bind(this))

        deviceList.appendChild(newCard);
        ElementControl.SetListLoading(false);

        return;
    }

    if (DeviceCard.GetDeviceCardCount() != this.deviceCount) {
        console.log("Device count does not match, re-fetching the list...");
        DeviceCard.ClearDeviceCard();
        ElementControl.SetListLoading(true);
        return;
    }

    var card = DeviceCard.FindDeviceCard(name);
        
    if (card == null) {
        console.log(`Cannot find the device with name ${name}`);
        DeviceCard.ClearDeviceCard();
        ElementControl.SetListLoading(true);
        return;
    }

    ElementControl.UpdateCardOnline(card, online);
    ElementControl.UpdateCardLoading(card, false);
  }

  onErrorCallback(error) {
    ElementControl.SetLoginPage();
    ElementControl.SetLoginStatus(false);
    console.log('MQTT error:', error);

      this.client.end(true, () => { });

      if (String(error).includes("Not authorized") || String(error).includes("Bad username or password")) {
        alert("不正確的使用者帳號或密碼");
      } else {
        alert(err);
      }

      clearInterval(this.refreshTimer);
  }

  onConnectCallback() {
      console.log("Successfully connected to the MQTT server");
      this.client.subscribe("esp32/wake");
      
    ElementControl.SetLoginStatus(false);
    ElementControl.SetDevicePage();
    ElementControl.SetListLoading(true);

    this.client.publish("esp32/wake", "{\"cmd\": \"query\"}");

    this.refreshTimer = setInterval(() => {
        this.client.publish("esp32/wake", "{\"cmd\": \"query\"}");
    }, 10000);
  }

}

function main() {
    const loginForm = document.getElementById('loginForm');

    const logoutBtn = document.getElementById('logoutBtn');

    var client = new Client();

    loginForm.addEventListener('submit', function(e) {
        e.preventDefault();

        const formData = new FormData(e.target);
        const data = Object.fromEntries(formData.entries());

        client.login(data.username, data.password);
        
    });

    logoutBtn.addEventListener('click', function() {
        client.logout();
        document.getElementById('appleID').value = '';
        document.getElementById('password').value = '';
    });
}

document.addEventListener('DOMContentLoaded', main);