/*!
 * @file client.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Implementation of Client
 */

#include "client.h"

#include <QUuid>
#include <QObject>
#include <QString>
#include <QPixmap> 
#include <QVariant>
#include <QByteArray>

#include "utils.h"

Client::Client(QObject *parent) : QObject(parent)
{
    options = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        .clean_start(false)
        .connect_timeout(std::chrono::milliseconds(5000))
        .finalize();
}

bool Client::connect(QString address)
{
    if(connected)
        return false;

    // Create client instance with unique ID
    connection = new mqtt::async_client(address.toStdString(), QUuid::createUuid().toString().toStdString(), mqtt::create_options(MQTTVERSION_5));
    
    // Register callbacks
    connection->set_disconnected_handler(std::bind(&Client::onDisconnected, this, std::placeholders::_1, std::placeholders::_2));
    connection->set_connection_lost_handler(std::bind(&Client::onConnectionLost, this, std::placeholders::_1));
    connection->set_message_callback(std::bind(&Client::onMessageReceived, this, std::placeholders::_1));

    try 
    {
        connection->connect(options)->wait();
    }
    catch(...) 
    {
        delete connection;
        return false;
    }

    connected = true;
    return true;
}

void Client::disconnect()
{
    if(!connected)
        return;

    if(connection->is_connected())
        connection->disconnect()->wait();
        
    connected = false;
    delete connection;
}

void Client::subscribe(QString topic)
{
    if(!connected || !connection->is_connected())
        return;

    connection->subscribe(topic.toStdString(), 1, mqtt::subscribe_options(true));
}

void Client::unsubscribe(QString topic)
{
    if(!connected || !connection->is_connected())
        return;

    connection->unsubscribe(topic.toStdString());
}

void Client::publish(QString topic, QString data)
{
    if(!connected || !connection->is_connected())
        return;

    connection->publish(topic.toStdString(), data.toStdString());
    emit receivedMessage(topic, data, true);
}

void Client::publish(QString topic, QByteArray data)
{
    if(!connected || !connection->is_connected())
        return;

    connection->publish(topic.toStdString(), data.data(), data.size());
    emit receivedMessage(topic, convertByteArray(data), true);
}

void Client::onDisconnected([[maybe_unused]] mqtt::properties properties, [[maybe_unused]] mqtt::ReasonCode reasonCode)
{
    qCritical("Connection unexpectedly terminated by server.");

    // we must exit because of the bug in Paho Eclipse library (most likely) 
    // we are not able to recover from this situation as the app would freeze forever
    // exit is absolutely not a good way to handle such situation (especially in multi threaded program)
    exit(1);
}

void Client::onConnectionLost([[maybe_unused]] mqtt::string reason)
{
    qCritical("Connection to server was lost.");

    // we must exit because of the bug in Paho Eclipse library (most likely) 
    // we are not able to recover from this situation as the app would freeze forever
    // exit is absolutely not a good way to handle such situation (especially in multi threaded program)
    exit(1);
}

void Client::onMessageReceived(mqtt::const_message_ptr message)
{
    if(!connected || !connection->is_connected())
        return;

    auto topic = QString::fromStdString(message->get_topic());
    auto data = QByteArray::fromRawData(message->get_payload().data(), message->get_payload().size());

    emit receivedMessage(topic, convertByteArray(data), false);
}

QVariant Client::convertByteArray(QByteArray data)
{
    // Only binary type that is supported is image, so try to parse it and it
    // If it is not image, create string from the data
    QVariant variant;
    QPixmap pixmap;
    if(pixmap.loadFromData(data))
        return pixmap;
    
    return QString(data);
}

Client::~Client()
{
    if(connected)
        delete connection;
}