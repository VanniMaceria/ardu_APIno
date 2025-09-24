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
- DC motor with fan

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
  <img src="https://github.com/user-attachments/assets/140c04b8-1026-4b70-ad9d-ae2b1f79d46a" width="700" height="400">
</p>

## Available endpoints
 ```
http://localhost:8080/sensor/dht - GET
```
provides temperature and humidity of DHT sensor connected to your micro-controller


```
http://localhost:8080/actuator/rgb_led - POST
```
turns on the LED with the 'r', 'g' and 'b' parameters (0 - 255)

```
http://localhost:8080/actuator/led - POST
```
turns on the LED with the intensity and color specified by the 'brightness' (0 - 255) parameter and 'color' (RED - GREEN - BLUE)

```
http://localhost:8080/actuator/fan_motor - POST
```
turns on the fan by using "speed" parameter (0 - 255)


## Swagger
By going to 
```
http://localhost:8080/index.html
```
you can access to the Swagger UI OpenAPI 
