# ardu_APIno

[![style: very good analysis][very_good_analysis_badge]][very_good_analysis_link]
[![License: MIT][license_badge]][license_link]
[![Powered by Dart Frog](https://img.shields.io/endpoint?url=https://tinyurl.com/dartfrog-badge)](https://dartfrog.vgv.dev)

A RESTful API written in Dart Frog for interacting with arduino/esp sensors and actuators

[license_badge]: https://img.shields.io/badge/license-MIT-blue.svg
[license_link]: https://opensource.org/licenses/MIT
[very_good_analysis_badge]: https://img.shields.io/badge/style-very_good_analysis-B22C89.svg
[very_good_analysis_link]: https://pub.dev/packages/very_good_analysis

## What you need
- ESP32 or similar;
- DHT sensor;
- LED

## How to use it
First, check [Dart Frog Docs](https://dartfrog.vgv.dev/docs/overview) to install Dart Frog.
Once you have finished run:
```bash
# 🏁 Start the dev server
dart_frog dev
```
in order to start the server on your localhost

## Workflow example
<p align="center">
  <img src="https://github.com/user-attachments/assets/3dd2e9ce-75cc-4360-859d-756c0e697afa" width="700" height="400">
</p>

## Available endpoints
 ```
http://localhost:8080/sensor/dht - GET
```
provides temperature and humidity of DHT sensor connected to your Arduino WiF


```
http://localhost:8080/actuator/led - POST
```
turns on the LED with the intensity specified by the 'voltage' parameter
