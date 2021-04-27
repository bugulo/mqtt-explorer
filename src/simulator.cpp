#include "simulator.h"

#include <random>

#include <QDebug>
#include <QUuid>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>

Simulator::Simulator(Explorer *explorer) : QObject(explorer)
{
    options = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        //.connect_timeout(5)
        .clean_start(true)
        .finalize();

    connect(&timerMain, SIGNAL(timeout()), this, SLOT(update()));
    connect(explorer, SIGNAL(disconnected()), this, SLOT(disconnected()));

    this->explorer = explorer;

    auto file = QFile("./simulator.json");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        explorer->setStatus("[Simulator] Configuration file not found");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        explorer->setStatus("[Simulator] Could not parse JSON file");
        return;
    }

    auto messages = doc.object().value("messages").toArray();
    foreach(const QJsonValue & message, messages)
    {
        auto msg = new Message();
        msg->topic = message.toObject().value("topic").toString();
        msg->timeout = message.toObject().value("timeout").toInt(1000);

        qDebug() << msg->topic;
        qDebug() << msg->timeout;

        auto options = message.toObject().value("options").toArray();
        foreach(const QJsonValue & option, options)
        {
            QString type = option.toObject().value("type").toString();
            QString content = option.toObject().value("content").toString();

            if(type == "string")
                msg->options.append(content);
            else if(type == "file")
            {
                auto imageFile = QFile(content);
                
                if(!imageFile.open(QIODevice::ReadOnly))
                {
                    explorer->setStatus("[Simulator] Failed to read file of provided image");
                    return;
                }

                auto imageData = imageFile.readAll();
                imageFile.close();
                msg->options.append(imageData);
            }
            else
            {
                explorer->setStatus("[Simulator] Unknow option type");
            }
        }

        registeredMessages.append(msg);
    }
}

void Simulator::disconnected()
{
    stop();
}

bool Simulator::start(mqtt::string address)
{
    if(running)
        return false;

    client = new mqtt::async_client(address, QUuid::createUuid().toString().toStdString(), mqtt::create_options(MQTTVERSION_5));

    try {
        client->connect(options)->wait();
    }
    catch(const std::exception& e) {
        delete client;
        return false;
    }

    running = true;

    timerMain.start(1000);
    explorer->setStatus("Simulator started!");
}

void Simulator::stop()
{
    if(!running)
        return;

    timerMain.stop();

    client->disconnect()->wait();

    running = false;
    explorer->setStatus("Simulator stopped!");
}

bool Simulator::isRunning()
{
    return running;
}

void Simulator::update()
{
    for(auto message : registeredMessages)
    {
        qDebug() << message->lastRun;
        qDebug() << QDateTime::currentDateTime();
        if(message->lastRun.addSecs(message->timeout) <= QDateTime::currentDateTime())
        {
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 gen(rd()); // seed the generator
            std::uniform_int_distribution<> distr(0, message->options.count() - 1); // define the range

            auto option = (int) distr(gen);

            auto selected = message->options[option];

            if(selected.userType() == QMetaType::QString)
                client->publish(message->topic.toStdString(), qvariant_cast<QString>(selected).toStdString());
            else if(selected.userType() == QMetaType::QByteArray)
            {
                auto data = qvariant_cast<QByteArray>(selected);
                client->publish(message->topic.toStdString(), data.data(), data.size());
            }

            message->lastRun = QDateTime::currentDateTime();
        }
    }
}