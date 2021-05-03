/*!
 * @file client.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Wrapper for Paho Eclipse MQTT client
 */

#ifndef E_CLIENT_H
#define E_CLIENT_H

#include <QString>
#include <QObject>
#include <QVariant>
#include <QByteArray>

#include "mqtt/async_client.h"

//! Eclipse Paho wrapper
class Client : public QObject
{
    Q_OBJECT

public:
    /*!
     * @brief Constructor
     * @param parent Pointer to the parent widget
     */
    Client(QObject *parent);

    //! Destructor
    ~Client();

    /*! 
     * @brief Connect to the specified server
     * @param address Hostname
     * @returns true when connected successfuly, false otherwise
     */
    bool connect(QString address);

    //! Disconnect from the server
    void disconnect();
    
    /*! 
     * @brief Subscibe topic
     * @param topic Topic name
     */
    void subscribe(QString topic);

    /*! 
     * @brief Unsubscribe topic
     * @param topic Topic name
     */
    void unsubscribe(QString topic);

    /*!
     * @brief Publish text to topic
     * @param topic Topic name
     * @param data Text
     */
    void publish(QString topic, QString data);

    /*!
     * @brief Publish ByteArray to topic
     * @param topic Topic name
     * @param data ByteArray
     */
    void publish(QString topic, QByteArray data);

signals:
    /*! 
     * @brief Signal that new message was received
     * @param topic Topic name
     * @param data Image or string
     * @param local Whether the message was sent by this client
     */
    void receivedMessage(QString topic, QVariant data, bool local);

private:
    /*! 
     * @brief Server disconnect callback
     * @param properties Properties
     * @param reasonCode Reason for disconnect
     */
    void onDisconnected(mqtt::properties properties, mqtt::ReasonCode reasonCode);

    /*! 
     * @brief Lost connection callback
     * @param reason Reason for disconnect
     */
    void onConnectionLost(mqtt::string reason);

    /*! 
     * @brief Message received callback
     * @param message Pointer to message data
     */
    void onMessageReceived(mqtt::const_message_ptr message);

    /*! 
     * @brief Convert ByteArray to image or string
     * @param data Byte array
     * @returns image or string
     */
    QVariant convertByteArray(QByteArray data);

    //! MQTT client instance
    mqtt::async_client *connection;

    //! Options to connect with
    mqtt::connect_options options;

    //! Whether the client is connected
    bool connected = false;
};


#endif