# 💊 MediBox - Smart Medicine Management System

## 🎯 What is MediBox?

MediBox is an intelligent medication management system that combines IoT technology with healthcare automation. It helps patients maintain their medication schedules through automated dispensing, environmental monitoring, and real-time alerts - all manageable through a user-friendly web dashboard.

### 🌟 Why MediBox?

- **Never Miss a Dose**: Automated dispensing with visual and audio reminders
- **Safe Storage**: Continuous monitoring of temperature, humidity, and light conditions
- **Remote Management**: Control and monitor from anywhere via web dashboard
- **Family Friendly**: Simple interface suitable for elderly patients and caregivers
- **Secure**: TLS-encrypted MQTT communication for data protection

---

## 📋 Features

### 🏥 Core Functionality
- **Automated Dispensing**: Servo-controlled mechanism releases medicine based on programmed schedules
- **Smart Scheduling**: Configure multiple daily doses with customizable timing
- **Manual Override**: Emergency dispensing through dashboard or physical buttons

### 📊 Monitoring & Analytics
- **Environmental Sensors**: Real-time temperature, humidity, and light intensity tracking
- **Historical Data**: Visualize medication adherence and environmental trends
- **Alert System**: Buzzer and LED notifications for reminders and warnings

### 🌐 Connectivity & Control
- **Web Dashboard**: Responsive Node-RED interface for complete system control
- **MQTT Integration**: Secure, real-time communication between device and dashboard
- **WiFi Connected**: Remote monitoring and control capabilities

### 🔧 Hardware Features
- **OLED Display**: Clear status information and menu navigation
- **Physical Controls**: Menu, OK, Up, Down, and Cancel buttons for local operation
- **Status Indicators**: LED and buzzer for visual and audio feedback
- **Robust Design**: ESP32-based system with reliable sensor integration

---

## 🛠️ Hardware Components

| Component | Purpose | Quantity |
|-----------|---------|----------|
| ESP32 Development Board | Main microcontroller | 1 |
| Servo Motor | Medicine dispensing mechanism | 1 |
| OLED Display (SSD1306) | Status display and menu | 1 |
| DHT22 Sensor | Temperature & humidity monitoring | 1 |
| LDR (Light Sensor) | Light intensity monitoring | 1 |
| Push Buttons | Menu navigation (Menu, OK, Up, Down, Cancel) | 5 |
| Buzzer | Audio alerts | 1 |
| LED | Visual indicators | 1 |

> 📄 **Detailed specifications and wiring diagrams are available in the [Hardware Documentation](Docs/Hardware/Hardware_Component_Specification.md)**

---

## 💻 Software Stack

- **[PlatformIO](https://platformio.org/)** - Development environment and build system
- **[Node-RED](https://nodered.org/)** - Dashboard and flow management
- **[MQTT Broker](https://mosquitto.org/)** - Message broker for IoT communication
- **[Wokwi Simulator](https://docs.wokwi.com/vscode/getting-started)** - Hardware simulation for testing

---

## 🚀 Quick Start

### Prerequisites
- ESP32 development board
- USB cable for programming
- WiFi network
- Computer with VSCode installed

### 1️⃣ Clone the Repository
```bash
git clone https://github.com/iransamarasekara/medibox.git
cd medibox
```

### 2️⃣ Hardware Setup
Connect components according to the [wiring diagram](Docs/Hardware/Hardware_Component_Specification.md#port-map).

### 3️⃣ Software Installation

#### Install Development Tools
1. Install [VSCode](https://code.visualstudio.com/)
2. Install [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
3. Install [Node-RED](https://nodered.org/docs/getting-started/installation)

#### Configure Security Certificate
Create `include/Certificate.h` with your CA certificate:
```cpp
const char *root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
...your certificate content here...
-----END CERTIFICATE-----
)EOF";
```

#### Update Configuration
Edit `include/Definitions.h` with your settings:
```cpp
#define WIFI_SSID "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_BROKER "your_mqtt_broker_address"
```

### 4️⃣ Build and Deploy

1. **Build the firmware**:
   ```bash
   pio run
   ```

2. **Upload to ESP32**:
   ```bash
   pio run --target upload
   ```

3. **Start Node-RED**:
   ```bash
   node-red
   ```

4. **Import Dashboard Flow**:
   - Open [http://localhost:1880](http://localhost:1880)
   - Import `NodeRed Dashboard/flow.json`

5. **Access Dashboard**:
   - Visit [http://localhost:1880/ui](http://localhost:1880/ui)

---

## 🖥️ Using the Dashboard

### Live Monitoring
- **Environmental Data**: Real-time temperature, humidity, and light readings
- **Device Status**: Current medicine levels, next scheduled dose, system health
- **Historical Charts**: Trend visualization for all monitored parameters

### Control Panel
- **Schedule Management**: Set up daily medication schedules
- **Manual Dispensing**: Emergency medicine release
- **System Settings**: Configure alerts, thresholds, and preferences
- **Maintenance Mode**: Calibration and testing functions

---

## 🧪 Simulation

Test your code without physical hardware using Wokwi:

1. Install [Wokwi extension](https://docs.wokwi.com/vscode/getting-started) for VSCode
2. Open `Simulation/diagram.json`
3. Run the simulation to test functionality

---

## 📖 Documentation

| Document | Description |
|----------|-------------|
| [Hardware Specifications](Docs/Hardware/Hardware_Component_Specification.md) | Complete component list and wiring diagrams |
| [Requirements](Docs/Requirements/) | System requirements and specifications |
| [API Reference](Docs/API/) | MQTT topics and message formats |
| [Troubleshooting](Docs/Troubleshooting.md) | Common issues and solutions |

---

## 🔧 Configuration

### WiFi Setup
Update credentials in `include/Definitions.h`:
```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"
```

### MQTT Configuration
Configure broker settings:
```cpp
#define MQTT_BROKER "your.mqtt.broker.com"
#define MQTT_PORT 8883
#define MQTT_USERNAME "your_username"
#define MQTT_PASSWORD "your_password"
```

### Sensor Calibration
Adjust sensor thresholds in the configuration file based on your environment.

---

## 🛡️ Security

- **TLS Encryption**: All MQTT communication is encrypted using TLS
- **Certificate Authentication**: Uses CA certificates for broker verification
- **Access Control**: Dashboard requires authentication for sensitive operations
- **Data Privacy**: All health data remains on your local network

---

## 🤝 Contributing

We welcome contributions! Here's how you can help:

### 🐛 Reporting Issues
- Use [GitHub Issues](https://github.com/iransamarasekara/medibox/issues) to report bugs
- Include detailed steps to reproduce the issue
- Attach relevant logs and screenshots

### 💡 Feature Requests
- Suggest new features through GitHub Issues
- Describe the use case and expected behavior
- Consider implementation complexity

### 🔧 Code Contributions
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### 📝 Documentation
- Help improve documentation
- Add examples and tutorials
- Translate to other languages

---

## 🎯 Roadmap

- [ ] **Mobile App**: Native iOS/Android application
- [ ] **Voice Control**: Integration with voice assistants
- [ ] **Multiple Compartments**: Support for multiple medicine types
- [ ] **Biometric Security**: Fingerprint authentication
- [ ] **Cloud Integration**: Optional cloud backup and synchronization
- [ ] **AI Predictions**: Smart scheduling based on usage patterns

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **ESP32 Community** for excellent documentation and examples
- **Node-RED Team** for the powerful dashboard framework
- **Arduino Community** for extensive library support
- **Healthcare Professionals** who provided valuable feedback during development

---

## 📞 Support

- **Documentation**: Check our [comprehensive docs](Docs/) first
- **Issues**: Report bugs via [GitHub Issues](https://github.com/iransamarasekara/medibox/issues)
- **Discussions**: Join community discussions in [GitHub Discussions](https://github.com/iransamarasekara/medibox/discussions)
- **Email**: [your.email@example.com](mailto:your.email@example.com) for direct support

---

## ⭐ Show Your Support

If MediBox helps you or your loved ones manage medication better, please consider:
- ⭐ Starring this repository
- 🍴 Forking and contributing
- 📢 Sharing with others who might benefit

---

<div align="center">

**[🔝 Back to Top](#-medibox---smart-medicine-management-system)**

*Stay healthy, stay smart with MediBox!*

</div>