#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import network
import time
import nvs

def connect_wifi(ota_ssid = None, ota_password = None, connect_timeout = 0xFFFF):
    if(ota_ssid == None) or (ota_password == None):
        ota_ssid = nvs.read(nvs.WIFI_SSID, nvs.OTA_NAMESPACE)
        ota_password = nvs.read(nvs.WIFI_PASSWORD, nvs.OTA_NAMESPACE)

    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.scan()
    wlan.isconnected()
    wlan.connect(ota_ssid, ota_password)
    print("connecting ...")
    wlan.config('mac')
    wlan.ifconfig()
    while (not wlan.isconnected()) and (connect_timeout):
        time.sleep(1)
        connect_timeout = connect_timeout - 1
        print("waiting ...")
    if(wlan.isconnected()):
        print('connect ok:', wlan.ifconfig())
        return 0
    else:
        print("connect fail. connect timeout")
        return 1

def set_wifi_name(wifi_ssid):
    ota_ssid = nvs.read(nvs.WIFI_SSID, nvs.OTA_NAMESPACE)
    if(ota_ssid != wifi_ssid):
        nvs.write(nvs.WIFI_SSID, wifi_ssid, nvs.OTA_NAMESPACE)

def set_wifi_password(wifi_password):
    ota_password = nvs.read(nvs.WIFI_PASSWORD, nvs.OTA_NAMESPACE)
    if ota_password != wifi_password:
        nvs.write(nvs.WIFI_PASSWORD, wifi_password, nvs.OTA_NAMESPACE)

def set_cloud_host(cloud_host):
    ota_host = nvs.read(nvs.OTA_HOST_NAME, nvs.OTA_NAMESPACE)
    if(ota_host != cloud_host):
        nvs.write(nvs.OTA_HOST_NAME, cloud_host, nvs.OTA_NAMESPACE)

def set_cloud_url(cloud_url):
    ota_url = nvs.read(nvs.OTA_INFO_URL, nvs.OTA_NAMESPACE)
    if(ota_url != cloud_url):
        nvs.write(nvs.OTA_INFO_URL, cloud_url, nvs.OTA_NAMESPACE)

def set_ota_ble(ota_ble_control):
    ota_ble = nvs.read(nvs.OTA_BLE, nvs.OTA_NAMESPACE)
    if(ota_ble != ota_ble_control):
        nvs.write(nvs.OTA_BLE, ota_ble_control, nvs.OTA_NAMESPACE)