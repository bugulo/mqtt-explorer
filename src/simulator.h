#ifndef E_SIMULATOR_H
#define E_SIMULATOR_H

#include <QTimer>

#include "mqtt/async_client.h"

#include "explorer.h"

class Explorer;

class Message
{
public:
    QDateTime lastRun = QDateTime::currentDateTime();
    int timeout;

    QString topic;

    QList<QVariant> options;
};

class Simulator : public QObject
{
    Q_OBJECT

public:
    Simulator(Explorer* explorer);

    bool start(mqtt::string address);
    void stop();

    bool isRunning();

private:
    mqtt::async_client *client;
    mqtt::connect_options options;

    QTimer timerMain;

    QList<Message*> registeredMessages;

    Explorer *explorer;

    bool running = false;

public slots:
    void update();
    void disconnected();
};

#endif