/*!
 * @file simulator.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Implementation of Simulator
 */

#include "simulator.h"

#include <random>

#include <QUuid>
#include <QObject>
#include <QDateTime>
#include <QMetaType>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonValueRef>

#include "utils.h"
#include "client.h"
#include "explorer.h"

Simulator::Simulator(Explorer *explorer) : QObject(explorer), explorer(explorer)
{
    client = new Client(this);

    connect(&updateTimer, &QTimer::timeout, this, &Simulator::onTimeout);
}

bool Simulator::loadConfiguration()
{
    auto data = Utils::loadFile(explorer, "Load simulator configuration", "All files (*)");

    auto document = QJsonDocument::fromJson(data);
    if (document.isNull()) 
    {
        explorer->setStatus("Failed to parse configuration file");
        return false;
    }

    // Result of topics parse
    // We check this variable after iteration and delete all parsed topics if failed
    auto parseResult = true;

    auto topicArray = document.object().value("topics").toArray();
    for(auto topicItem : topicArray)
    {
        auto topic = new SimulatorTopic();
        topic->name = topicItem.toObject().value("name").toString();
        topic->period = topicItem.toObject().value("period").toInt();

        // Topic with empty name or negative period would break things
        if(topic->name.isEmpty() || topic->period < 1)
        {
            parseResult = false;
            break;
        }

        auto messageArray = topicItem.toObject().value("messages").toArray();
        for(auto messageItem : messageArray)
        {
            auto type = messageItem.toObject().value("type").toString();
            auto content = messageItem.toObject().value("content").toString();

            // Message type is string, we can append it as it is
            if(type == "string")
                topic->messages.append(content);
            // Message type is file, we need to load contents of that file before appending
            else if(type == "file")
                topic->messages.append(Utils::readFile(content));
            // Only string and file types are supported
            else 
            {
                parseResult = false;
                break;
            }
        }

        if(!parseResult)
            break;

        topics.append(topic);
    }

    if(!parseResult)
    {
        explorer->setStatus("Failed to parse configuration file");

        // Delete all topics that were parsed successfuly
        for(auto topic : topics)
            delete topic;

        return false;
    }

    return true;
}

bool Simulator::start(QString address)
{
    if(running)
        return false;

    // Prompt user to select configuration file when the Simulator is first started
    if(!loaded)
    {
        if(loadConfiguration())
            loaded = true;
        else
            return false;
    }

    if(!client->connect(address))
    {
        explorer->setStatus("Simulator failed to connect to the server");
        return false;
    }

    running = true;

    updateTimer.start(1000);
    explorer->setStatus("Simulator started!");
    return true;
}

void Simulator::onTimeout()
{
    for(auto topic : topics)
    {
        // Ignore topics that Simulator already published to in this period 
        if(topic->lastSendTime.addSecs(topic->period) > QDateTime::currentDateTime())
            continue;

        // Create random number generator
        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<> distribution(0, topic->messages.count() - 1);

        // Select random message from provided list
        auto message = topic->messages[(int) distribution(generator)];

        // Only string and ByteArray types are supported
        if(message.userType() == QMetaType::QString)
            client->publish(topic->name, qvariant_cast<QString>(message));
        else if(message.userType() == QMetaType::QByteArray)
            client->publish(topic->name, qvariant_cast<QByteArray>(message));
        else 
            continue;

        topic->lastSendTime = QDateTime::currentDateTime();
    }
}

void Simulator::stop()
{
    if(!running)
        return;

    running = false;
    updateTimer.stop();

    client->disconnect();
    explorer->setStatus("Simulator stopped!");
}

bool Simulator::isRunning()
{
    return running;
}

Simulator::~Simulator()
{
    delete client;
    for(auto topic : topics)
        delete topic;
}