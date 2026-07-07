# 🎛️ SensorPod
> A super-modular IoT sensor node built around the ESP32 and Atmega328, designed for Smart Irrigation / Smart Farming.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)
![Framework](https://img.shields.io/badge/framework-PlatformIO%20%7C%20Arduino-orange.svg)

## 📖 Overview

**SensorPod** is an *CHEAP & MODULAR* Smart Irrigation / Smart Garden project that's focused on Farming utilities especially in Indonesia.

## 🛠️ Hardware Requirements

- **Atmega328 Pro Mini (3.3V 8MHz )** 
- **2x Capacitive Moisture Sensor V2**
- **MAX3485 RS485 Protocol**
- **3-12v - 3v Regulator** 

## 🚀 Features
- 2 Capacitive Soil Moisture Sensor V2
- RS485 Communication
- Configurable Node ID
- Low cost
- Designed for PlantPod ecosystem

### Structure
src/
   app/ <br/>
   Application Logic <br/>
   
   communication/ <br/>
   UART to RS485 Protocol Communication <br/>

   drivers/ <br/>
   Hardware Drivers (Sensor) <br/>   

   config/ <br/>
   Configuration <br/>

   main.cpp <br/>

### Roadmap
✔ RS485 <br/>
✔ Multi Node <br/>
✔ Node ID <br/>
✔ EEPROM <br/>
□ Calibration <br/>
□ CRC <br/>
□ OTA (future) <br/>
□ Sleep mode <br/>

### Installation
1. **Clone the repository**
   ```bash
   git clone https://github.com/noelleputra/SensorPod.git
   cd SensorPod
   ```

## 🔌 Pin Configuration
This project still under development!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/NewSensorModule`)
3. Commit your Changes (`git commit -m 'Add support for new sensor module'`)
4. Push to the Branch (`git push origin feature/NewSensorModule`)
5. Open a Pull Request

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.