#include <QApplication>
#include <QLabel>
#include <QDebug>

#include "mqtt/async_client.h"

#include "explorer.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Explorer explorer;
    explorer.show();

    /*QApplication::instance();

    Connect connect;
    connect.show();

    qDebug() << connect.clientText->text();
    qDebug() << connect.serverText->text();

    mqtt::async_client client(connect.serverText->text().toStdString(), connect.clientText->text().toStdString());

    mqtt::connect_options options;
    options.set_clean_session(true);

    client.set_message_callback([](mqtt::const_message_ptr message) {
        qDebug() << "Message arrived";
        qDebug() << QString::fromStdString(message->get_topic());
        qDebug() << QString::fromStdString(message->to_string());
    });

    try
    {
        qDebug() << "Connecting to MQTT server";
        client.connect(options)->wait();
    }
    catch(const mqtt::exception& e)
    {
        qDebug() << "Failed to connect to MQTT server";
        return 1;
    }

    qDebug() << "Connected";

    client.subscribe("hello", 1)->wait();
    client.publish("hello", "WTFFF")->wait();

    Explorer explorer;
    explorer.show();*/

    return app.exec();
}