| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----- |

#Get water lever project
By using Lazer sensor, get water lever and post to server

#problemps:
1. Lazer sensor deed ~5s from power on to warm up
2. Hall UART lib: Can not read data response from lazer-sensor
->HW-Uart used to read data in string, need to config it to hex data