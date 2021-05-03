!versionAtLeast(QT_VERSION, 5.12.0) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 5.12 or newer")
}

TARGET = mqtt-explorer
CONFIG += c++17
QT += core gui widgets

# Link Eclipse Paho
LIBS += -lpaho-mqtt3c -lpaho-mqtt3a -lpaho-mqttpp3

SOURCES += \
    src/main.cpp \
    src/utils.cpp \
    src/client.cpp \
    src/explorer.cpp \
    src/simulator.cpp \
    src/widgets/widget.cpp \
    src/widgets/thermostat/thermostat.cpp \
    src/widgets/lcd_display/lcd_display.cpp \
    src/widgets/light_switch/light_switch.cpp \
    src/widgets/security_camera/security_camera.cpp \
    src/extensions/FlowLayout.cpp 

HEADERS += \
    src/utils.h \
    src/client.h \
    src/explorer.h \
    src/simulator.h \
    src/widgets/widget.h \
    src/widgets/thermostat/thermostat.h \
    src/widgets/lcd_display/lcd_display.h \
    src/widgets/light_switch/light_switch.h \
    src/widgets/security_camera/security_camera.h \
    src/extensions/FlowLayout.h

FORMS += \
    src/explorer.ui \
    src/preview_image.ui \
    src/preview_string.ui \ 
    src/widgets/light_switch/light_switch.ui \
    src/widgets/light_switch/light_switch_settings.ui \
    src/widgets/lcd_display/lcd_display.ui \
    src/widgets/lcd_display/lcd_display_settings.ui \
    src/widgets/security_camera/security_camera.ui \
    src/widgets/security_camera/security_camera_settings.ui \
    src/widgets/thermostat/thermostat.ui \
    src/widgets/thermostat/thermostat_settings.ui

RESOURCES += Resources.qrc