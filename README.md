# flood-moniter-system-

1. Aim
To design and simulate an IoT-based flood monitoring system that continuously measures water levels at a dam and provides real-time alerts to authorities via the cloud when flow levels exceed a predefined threshold, indicating potential flooding risks.

2. Problem Statement
Floods pose a significant threat to communities near dams, often resulting in property damage and loss of life. Traditional monitoring methods lack real-time responsiveness and remote accessibility. This project aims to create a system that uses sensors to monitor water levels continuously, enabling instant alerts and timely preventive action when dangerous conditions are detected.

3. Scope of the Solution
Continuous, automated monitoring of water level at a dam using an ultrasonic sensor.

Real-time data transmission and alerting via a web-based dashboard.

Visual (LED) and web-based notification when water exceeds a safe threshold.

Entire system simulated using Wokwi (no physical hardware required).

Documentation and demonstration provided via GitHub and video.

4. Required Components
Software
Wokwi Online Simulator: For simulating the circuit and microcontroller code.

Arduino/ESP32 Code: Written in C++ (compatible with Wokwi and PlatformIO).

GitHub: For project documentation, code, and sharing assets.

Web Browser: For viewing the dashboard and running the simulation.

Hardware (Virtual)
ESP32 Microcontroller (recommended for Wi-Fi/web server features)

HC-SR04 Ultrasonic Sensor: Measures water level.

LEDs: Green for safe, red for alert.

(Optional) Buzzer for audible alert.

Cloud/Network Environment
Local HTTP Server: Hosted on ESP32 (simulated in Wokwi), accessible via browser.

(Optional) MQTT broker or cloud endpoint for advanced alerting (not required for basic simulation).


5. Flowchart of the Code
[Start]
   |
[Initialize WiFi, Pins, Sensor]
   |
[Loop]
   |
[Read water level from ultrasonic sensor]
   |
[Is water level > threshold?] --No--> [Green LED ON, Red LED OFF, Status: Normal] --> [Wait & Repeat]
   |
  Yes
   |
[Red LED ON, Green LED OFF, Status: ALERT]
   |
[Send alert to web dashboard]
   |
[Wait & Repeat]
