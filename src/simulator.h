/*!
 * @file simulator.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Traffic simulation
 */

#ifndef E_SIMULATOR_H
#define E_SIMULATOR_H

#include <QDateTime>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QList>

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
    
    //! How often is message published to the topic (in seconds)
    int period;

    //! Last time a message was published to this topic
    QDateTime lastSendTime = QDateTime::currentDateTime();

    //! List of possible messages to this topic
    QList<QVariant> messages;
};

/*!
 * @brief Simulation of traffic on MQTT server
 * 
 * Simulator loads configuration from JSON file and then sends random message from list of messages every period
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
     * @brief Start the simulator and connect it to specified MQTT server
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

private slots:
    //! This slot is called on every updateTimer tick
    void onTimeout();

private:
    /*! 
     * @brief Open FileDialog so user can select configuration file containing list of messages 
     * @returns true if the file was parsed successfuly, false if user did not select file or provided invalid file
     */
    bool loadConfiguration();

    //! Explorer instance
    Explorer *explorer;

    //! Client instance
    Client *client;

    //! Timer that runs every second
    QTimer updateTimer;

    //! List of all registered topics
    QList<SimulatorTopic*> topics;

    //! Whether the Simulator is running
    bool running = false;

    //! Whtether user already provided configuration file
    bool loaded = false;
};

#endif