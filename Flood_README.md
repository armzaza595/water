# 🌊 Flood Monitoring System (ESP8266 + Telegram + Web Dashboard)

## 📌 Overview
ระบบตรวจจับระดับน้ำแบบ Real‑time โดยใช้ ESP8266 และ Ultrasonic Sensor พร้อมแจ้งเตือนผ่าน Telegram, Buzzer และ Web Dashboard

---

## ⚙️ Features
- วัดระดับน้ำแบบ Real‑time
- แจ้งเตือนผ่าน Telegram Bot
- Web Dashboard แสดงผลผ่าน Browser
- Buzzer แจ้งเตือนเมื่อระดับน้ำ ≥ 40 cm
- แสดงเวลาและประวัติการแจ้งเตือน

---

## 🔌 Hardware
- ESP8266 (NodeMCU / Wemos D1 Mini)
- Ultrasonic Sensor HC‑SR04
- Buzzer

---

## 🔧 Pin Connection

| ESP8266 | Device |
|--------|--------|
| D1 | TRIG |
| D2 | ECHO |
| D5 | BUZZER |

---

## 🌐 Web Access

เปิด Browser:
http://DEVICE_IP

---

## 🤖 Telegram

ระบบจะส่งข้อความเมื่อระดับน้ำเปลี่ยน

ตัวอย่าง:
ระดับน้ำ: 35 cm

---

## 🚀 Installation

1. เปิด Arduino IDE
2. ติดตั้ง ESP8266 board
3. ติดตั้ง libraries:
   - ESP8266WiFi
   - UniversalTelegramBot
   - ESP8266WebServer
4. Upload code

---

## 👨‍💻 Author
Flood Alert System Project
