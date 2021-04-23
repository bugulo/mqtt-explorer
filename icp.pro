TARGET = mqtt-explorer
CONFIG += c++17 debug
QT += core gui widgets

# Link Eclipse Paho
LIBS += -lpaho-mqtt3c -lpaho-mqtt3a -lpaho-mqttpp3

SOURCES += src/main.cpp