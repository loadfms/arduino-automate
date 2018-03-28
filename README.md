

![loadfms](https://img.shields.io/badge/with%20love-loadfms-red.svg)
# Automate

Simple project to turn on and off any thing connected to relays by querying web-api.

## Components
- Arduino Uno
- Ethernet card ENC28J60
- Relays
- Protoboard

## Build
To connect ENC28J60 to Arduino, please, follow this diagram:
![diagram](https://www.filipeflop.com/wp-content/uploads/2014/06/Ethernet_ENC28J602.jpg)

## Library
Included in project.

## How it works
Arduino execute a request to api and it responds if that relay should be open or not.