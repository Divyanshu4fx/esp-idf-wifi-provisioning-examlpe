# ESP32 WiFi Provisioning and Cat Fact Fetcher

## Overview

This project demonstrates WiFi provisioning on an ESP32 using Bluetooth Low Energy (BLE) and fetches a random cat fact once connected to the internet. The application showcases:
- WiFi provisioning using BLE
- HTTP GET request to fetch data
- FreeRTOS task management
- Error handling and connection management

## Prerequisites

### Hardware
- ESP32 development board
- USB Cable
- Computer with:
  - ESP-IDF installed
  - Bluetooth capabilities

### Software
- ESP-IDF (Espressif IoT Development Framework)
- Python
- Git
- VSCode

## Setup Instructions

### 1. Install ESP-IDF

#### Option 1: Manual Installation
1. Clone ESP-IDF repository
```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
```

2. Install ESP-IDF tools
```bash
./install.sh
```

3. Set up environment variables
```bash
. ./export.sh
```

### 2. Project Setup

1. Create a new project directory
```bash
mkdir cat_fact_provisioner
cd cat_fact_provisioner
```

2. Create project structure
```
cat_fact_provisioner/
│
├── main/
│   └── Assignment_project.c
├── CMakeLists.txt
└── sdkconfig
```

3. Copy the provided source code into `main/Assignment_project.c`

### 3. Configure Project

1. Use `idf.py menuconfig`
2. Navigate and enable:
   - Bluetooth
   - WiFi
   - HTTP Client
   - NVS storage

### 4. Build and Flash

1. Connect ESP32 via USB
2. Verify port (typically `/dev/ttyUSB0`)
3. Build and flash
```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash
idf.py monitor
```

## Provisioning Process

### Using BLE Provisioning

1. Device starts in unprovisioned state
2. Use ESP SoftAP or BLE provisioning app
   - Android/iOS: ESP BLE Provisioning App

### Provisioning Steps

1. Open provisioning app
2. Select "PROV_ESP32" network
3. Enter Password: `12345678`
4. Select WiFi network
5. Enter WiFi credentials

## Code Walkthrough

### Key Components

- `start_wifi_provisioning()`: Initiates BLE provisioning
- `event_handler()`: Manages WiFi connection events
- `rest_get()`: Fetches cat fact via HTTP
- `wifi_task()`: Monitors WiFi connection

### Error Handling

- NVS (Non-Volatile Storage) initialization
- WiFi connection retry mechanism
- Graceful error logging

## Troubleshooting

### Common Issues

1. **No WiFi Connection**
   - Verify provisioning credentials
   - Check network signal strength
   - Restart device

2. **BLE Provisioning Failure**
   - Ensure Bluetooth is enabled
   - Update ESP-IDF
   - Check device compatibility

3. **HTTP Request Failure**
   - Verify internet connectivity
   - Check API endpoint status

### Debugging Tips

- Use `idf.py monitor` to view logs
- Check serial output for detailed messages
- Verify WiFi credentials

## Dependencies

- ESP-IDF v4.4+
- FreeRTOS
- ESP HTTP Client
- WiFi Provisioning Library
