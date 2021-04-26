TARGET = mqtt-explorer
CONFIG += c++17 debug
QT += core gui widgets

# Link Eclipse Paho
LIBS += -lpaho-mqtt3c -lpaho-mqtt3a -lpaho-mqttpp3

SOURCES += src/main.cpp src/explorer.cpp src/simulator.cpp
HEADERS += src/explorer.h src/simulator.h

FORMS += src/widgets/explorer.ui src/widgets/preview_image.ui src/widgets/preview_string.ui