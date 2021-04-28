/*!
 * @file client.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Implementation of Client
 */

#include "client.h"

#include <QUuid>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QByteArray>

Client::Client(QObject *parent) : QObject(parent)
{
    options = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        .clean_start(true)
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
    catch(const std::exception& e) 
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

    connection->disconnect()->wait();
    connected = false;
    delete connection;
}

bool Client::subscribe(QString topic)
{
    if(!connected || !connection->is_connected())
        return false;

    connection->subscribe(topic.toStdString(), 1, mqtt::subscribe_options(true));
    return true;
}

bool Client::unsubscribe(QString topic)
{
    if(!connected || !connection->is_connected())
        return false;

    connection->unsubscribe(topic.toStdString());
    return true;
}

bool Client::publish(QString topic, QString data)
{
    if(!connected || !connection->is_connected())
        return false;

    connection->publish(topic.toStdString(), data.toStdString());
    emit receivedMessage(topic, data.toUtf8(), true);
    return true;
}

bool Client::publish(QString topic, QByteArray data)
{
    if(!connected || !connection->is_connected())
        return false;

    connection->publish(topic.toStdString(), data.data(), data.size());
    emit receivedMessage(topic, data, true);
    return true;
}

void Client::onDisconnected([[maybe_unused]] mqtt::properties properties, [[maybe_unused]] mqtt::ReasonCode reasonCode)
{
    if(!connected)
        return;

    connected = false;
    emit disconnected(DisconnectReason::TerminatedByServer);
    delete connection;
}

void Client::onConnectionLost([[maybe_unused]] mqtt::string reason)
{
    if(!connected)
        return;

    connected = false;
    emit disconnected(DisconnectReason::LostConnection);
    delete connection;
}

void Client::onMessageReceived(mqtt::const_message_ptr message)
{
    if(!connected || !connection->is_connected())
        return;

    auto topic = QString::fromStdString(message->get_topic());
    auto data = QByteArray::fromRawData(message->get_payload().data(), message->get_payload().length());
    emit receivedMessage(topic, data, false);
}

Client::~Client()
{
    delete connection;
}