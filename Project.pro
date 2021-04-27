TARGET = mqtt-explorer
CONFIG += c++17 debug
QT += core gui widgets

# Link Eclipse Paho
LIBS += -lpaho-mqtt3c -lpaho-mqtt3a -lpaho-mqttpp3

SOURCES += \
    src/main.cpp \
    src/explorer.cpp \
    src/simulator.cpp \
    src/widgets/widget.cpp \
    src/widgets/light_switch/light_switch.cpp \
    src/extensions/QFlowLayout.cpp 

HEADERS += \
    src/explorer.h \
    src/simulator.h \
    src/widgets/widget.h \ 
    src/widgets/light_switch/light_switch.h \
    src/extensions/QFlowLayout.h

FORMS += \
    src/explorer.ui \
    src/preview_image.ui \
    src/preview_string.ui \ 
    src/widgets/light_switch\light_switch.ui \
    src/widgets/light_switch\light_switch_settings.ui 

RESOURCES += Resources.qrc