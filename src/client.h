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
#include <QByteArray>

#include "mqtt/async_client.h"

//! Reason of client disconnect
enum DisconnectReason
{
    //! User terminated the connection
    TerminatedByUser, 
    //! Server terminated the connection
    TerminatedByServer,
    //! Client lost connection to the server
    LostConnection,
    //! Unknown error
    Unknown
};

Q_DECLARE_METATYPE(DisconnectReason);

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
     */
    bool connect(QString address);

    //! Disconnect from the server
    void disconnect();
    
    /*! 
     * @brief Subscibe to topic
     * @param topic Topic name
     */
    bool subscribe(QString topic);

    /*! 
     * @brief Unsubscribe topic
     * @param topic Topic name
     */
    bool unsubscribe(QString topic);

    /*! 
     * @brief Publish string
     * @param data Data to publish
     */
    bool publish(QString topic, QString data);

    /*! 
     * @brief Publish raw data
     * @param data Data to publish
     */
    bool publish(QString topic, QByteArray data);

signals:
    /*! 
     * @brief The signal is emitted when the client was disconnected from server
     * @param reason Reason for disconnect
     */
    void disconnected(DisconnectReason reason);

    /*! 
     * @brief The signal is emitted when the client received message
     * @param topic Topic name
     * @param data Raw data
     * @param local Whether the message was sent by this client
     */
    void receivedMessage(QString topic, QByteArray data, bool local);

private:
    //! Server disconnect callback
    void onDisconnected(mqtt::properties properties, mqtt::ReasonCode reasonCode);

    //! Lost connection callback
    void onConnectionLost(mqtt::string reason);

    //! Message received callback
    void onMessageReceived(mqtt::const_message_ptr message);

    //! MQTT client instance
    mqtt::async_client *connection;

    //! Options to connect with
    mqtt::connect_options options;

    //! Whether the client is connected
    bool connected = false;
};


#endif