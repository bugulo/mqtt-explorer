/*!
 * @file simulator.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief MQTT traffic simulation
 */

#ifndef E_SIMULATOR_H
#define E_SIMULATOR_H

#include <QDateTime>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QList>

#include "mqtt/async_client.h"

#include "explorer.h"
#include "client.h"

// Forward declarations
class Explorer;
class Client;

//! Stores topic information used in Simulator
class SimulatorTopic
{
public:
    //! Topic name
    QString name;
    
    //! How often should simulator publish message to the topic (in seconds)
    int period;

    //! When was the last message published
    QDateTime lastSendTime = QDateTime::currentDateTime();

    //! List of messages that Simulator chooses randomly from
    QList<QVariant> messages;
};

/*!
 * @brief MQTT traffic simulator
 * 
 * User is prompted to select configuration file in JSON format when simulator is first started.
 * Topics are parsed from such file into SimulatorTopic instances.
 * Simulator then publishes random message on specified period for every topic.
 */
class Simulator : public QObject
{
    Q_OBJECT

public:
    /*!
     * @brief Constructor
     * @param explorer Pointer to explorer instance
     */
    Simulator(Explorer* explorer);

    //! Destructor
    ~Simulator();

    /*!
     * @brief Start the simulator on connect it to specified MQTT server
     *
     * When this function is called the first time, user is prompted to select configuration file
     * 
     * @param address Hostname
     * @returns false if user did not select configuration file, connection to server failed or the Simulator is already running, 
     *          true otherwise
     */
    bool start(QString address);

    //! Stop the Simulator and disconnect it from the server
    void stop();

    //! This property holds whether the Simulator is running 
    bool isRunning();

signals:
    //! The signal is emitted when Simulator was unexpectedly disconnected from server
    void crashed();

private slots:
    /*! 
     * @brief Callback for updateTimer timeout signal
     *
     * Iterates all registered topics and publishes random message from provided options.
     */
    void onTimeout();


    void onClientDisconnected(DisconnectReason reason);

    /*! 
     * @brief Callback for Explorer disconnected signal
     *
     * Stops the simulator when client disconnected in Explorer
     */
    void onExplorerDisconnected();

private:
    /*! 
     * @brief Prompts user to select configuration file that will be parsed 
     * @returns true if the file was parsed successfuly, false if user did not select file or provided invalid file
     */
    bool loadConfiguration();

    /*!
     * @brief Called by MQTT client when server closed the connection
     * @param properties MQTT property list
     * @param reasonCode Reason of disconnect
     */
    void onDisc(mqtt::properties properties, mqtt::ReasonCode reasonCode);

    /*!
     * @brief Called by MQTT client when client lost the connection
     */
    void onConnectionLost(mqtt::string reason);

    //! Explorer instance
    Explorer *explorer;

    //! Client instance
    Client *client;

    //! Timer that runs periodically
    QTimer updateTimer;

    //! List of all registered topics
    QList<SimulatorTopic*> topics;

    //! State of Simulator
    bool running = false;

    //! If user already provided configuration file
    bool loaded = false;
};

#endif