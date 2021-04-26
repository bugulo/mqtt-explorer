#include "simulator.h"

#include <QDebug>
#include <QUuid>

Simulator::Simulator()
{
    options = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        //.connect_timeout(5)
        .clean_start(true)
        .finalize();

    connect(&timerMain, SIGNAL(timeout()), this, SLOT(update()));
}

bool Simulator::start(mqtt::string address)
{
    client = new mqtt::async_client(address, QUuid::createUuid().toString().toStdString(), mqtt::create_options(MQTTVERSION_5));

    try {
        client->connect(options)->wait();
    }
    catch(const std::exception& e) {
        delete client;
        return false;
    }

    timerMain.start(2000);
}

void Simulator::stop()
{
    client->disconnect()->wait();

    timerMain.stop();
}

void Simulator::update()
{
    client->publish("test", "ratemal");
}