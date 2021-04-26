#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QTimer>

#include "mqtt/async_client.h"

class Simulator : public QObject
{
    Q_OBJECT

public:
    Simulator();

    bool start(mqtt::string address);
    void stop();

private:
    mqtt::async_client *client;
    mqtt::connect_options options;

    QTimer timerMain;

public slots:
    void update();
};

#endif