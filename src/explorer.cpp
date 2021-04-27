#include "explorer.h"

#include <string>
#include <random>

#include <QDebug>
#include <QUuid>
#include <QFileDialog>
#include <QBuffer>
#include <QFileInfo>
#include <QFile>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSaveFile>

#include "mqtt/async_client.h"

#include "ui_preview_image.h"
#include "ui_preview_string.h"

#include "widgets/light_switch/light_switch.h"

#include "extensions/QFlowLayout.h"

#include "widgets/widget.h"
#include "simulator.h"

const int MAX_MESSAGES = 5;

Explorer::Explorer(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    options = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        //.connect_timeout(5)
        .clean_start(true)
        .finalize();

    // Setup button click events
    connect(buttonSubscribe,        SIGNAL(clicked()), this, SLOT(onSubscribeButtonClicked()));
    connect(buttonSaveState,        SIGNAL(clicked()), this, SLOT(onSaveStateButtonClicked()));
    connect(buttonToggleSubscribe,  SIGNAL(clicked()), this, SLOT(onToggleButtonClicked()));
    connect(buttonConnect,          SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    connect(buttonPublish,          SIGNAL(clicked()), this, SLOT(onPublishButtonClicked()));
    connect(buttonDisconnect,       SIGNAL(clicked()), this, SLOT(onDisconnectButtonClicked()));
    connect(buttonPublishImage,     SIGNAL(clicked()), this, SLOT(onPublishImageButtonClicked()));
    connect(buttonToggleSimulator,  SIGNAL(clicked()), this, SLOT(onToggleSimulatorButtonClicked()));

    // Setup tree widget selection event
    connect(topicTree, SIGNAL(itemSelectionChanged()), this, SLOT(onTopicSelected()));

    // Setup message click event in message history
    connect(topicMessageList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onMessageDoubleClicked(QListWidgetItem*)));

    connect(buttonAddWidget,        SIGNAL(clicked()), this, SLOT(onAddWidgetButtonClicked()));
    connect(buttonLoadDashboard,    SIGNAL(clicked()), this, SLOT(onLoadDashboardButtonClicked()));
    connect(buttonSaveDashboard,    SIGNAL(clicked()), this, SLOT(onSaveDashboardButtonClicked()));

    // Hide column that stores topic path
    topicTree->hideColumn(2);

    simulator = new Simulator(this);
    //simulator->start("test.mosquitto.org:1883");

    //verticalLayout_3->setAlignment(Qt::AlignTop);
    flowLayout = new FlowLayout();
    scrollAreaWidgetContents_2->setLayout(flowLayout);

    registeredWidgets["LightSwitch"] = new WidgetFactory<WidgetLightSwitch>;
}

void Explorer::setStatus(QString message, int seconds)
{
    statusBar()->showMessage(message, seconds * 1000);
}

void Explorer::onConnectButtonClicked()
{
    topicTree->clear();
    // Reset state of some buttons as topic needs to be selected before being able to push those buttons
    buttonToggleSimulator->setText("Start simulator on this server");
    inputPublishMessage->setEnabled(false);
    buttonToggleSubscribe->setEnabled(false);
    buttonPublishImage->setEnabled(false);
    buttonPublish->setEnabled(false);

    // Create client instance with unique ID
    client = new mqtt::async_client(this->inputServerAddress->text().toStdString(), QUuid::createUuid().toString().toStdString(), mqtt::create_options(MQTTVERSION_5));
    // Register callbacks
    client->set_message_callback(std::bind(&Explorer::onMessageReceived, this, std::placeholders::_1));
    client->set_connection_lost_handler(std::bind(&Explorer::onConnectionLost, this, std::placeholders::_1));
    client->set_disconnected_handler(std::bind(&Explorer::onDisconnected, this, std::placeholders::_1, std::placeholders::_2));

    try {
        setStatus("Connecting to server...", 0);
        client->connect(options)->wait();
    }
    catch(const std::exception& e) {
        setStatus("Could not connect to this server", 3);
        delete client;
        return;
    }

    buttonConnect->setEnabled(false);
    buttonDisconnect->setEnabled(true);
    inputServerAddress->setEnabled(false);
    tabWidget->setEnabled(true);
    setStatus("Successfuly connected to server!", 3);
}

void Explorer::onMessageReceived(mqtt::const_message_ptr message)
{
    auto topic = QString::fromStdString(message->get_topic());

    // Retrieve payload as raw bytes
    auto data = QByteArray::fromRawData(message->get_payload().data(), message->get_payload().length());

    // Only binary type that is supported is image, so try to parse it and it
    // If it is not image, create string from the data
    QPixmap pixmap;
    if(pixmap.loadFromData(data))
        processTopicMessage(topic, pixmap);
    else
        processTopicMessage(topic, QString::fromStdString(message->to_string()));
}

void Explorer::onConnectionLost([[maybe_unused]] mqtt::string reason)
{
    delete client;
    buttonConnect->setEnabled(true);
    buttonDisconnect->setEnabled(false);
    inputServerAddress->setEnabled(true);
    tabWidget->setEnabled(false);
    emit disconnected();
    setStatus("Connection to the server was lost", 3);
}

void Explorer::onDisconnected([[maybe_unused]] mqtt::properties properties, [[maybe_unused]] mqtt::ReasonCode reasonCode)
{
    delete client;
    buttonConnect->setEnabled(true);
    buttonDisconnect->setEnabled(false);
    inputServerAddress->setEnabled(true);
    tabWidget->setEnabled(false);
    emit disconnected();
    setStatus("Connection was closed by the server", 3);
}

void Explorer::onDisconnectButtonClicked()
{
    client->disconnect()->wait();
    delete client;

    buttonConnect->setEnabled(true);
    buttonDisconnect->setEnabled(false);
    inputServerAddress->setEnabled(true);
    tabWidget->setEnabled(false);
    emit disconnected();
    setStatus("Successfuly disconnected from server!", 3);
}

void Explorer::onSubscribeButtonClicked()
{
    int result;
    
    // If relative subscription is turned on, get selected topic, otherwise working with root
    if(checkboxRelative->isChecked())
    {
        auto selected = getSelectedTopic();
    
        if(selected == NULL)
        {
            setStatus("No topic is selected");
            return;
        }

        result = subscribeTopic(inputSubscribeTopic->text(), selected);
    } else
        result = subscribeTopic(inputSubscribeTopic->text());

    if(result == 1)
    {
        setStatus("You most provide topic before trying to subscribe!", 3);
        return;
    }
    else if(result == 2)
    {
        setStatus("This topic is already subscribed", 3);
        return;
    }
    else
        inputSubscribeTopic->setText("");
}

int Explorer::subscribeTopic(QString topic, QTreeWidgetItem *root)
{
    // Split specified topic by subtopics, so we can build tree structure
    auto path = topic.split("/", QString::SkipEmptyParts);

    if(path.count() == 0) 
        return 1;

    auto current = root == NULL ? topicTree->invisibleRootItem() : root;

    for(auto path_i = 0; path_i < path.count(); path_i++) 
    {
        auto found = false;
        // If partial path exists, iterate existing children
        for(auto child_i = 0; child_i < current->childCount(); child_i++)
        {
            auto child = current->child(child_i);
            if(child->text(0) == path[path_i])
            {
                current = child;
                found = true;
                break;
            }
        }
        
        // We found existing partial path, we do not have to create this element again
        if(found)
            continue;

        TopicData topicData;
        topicData.widgetItem = new QTreeWidgetItem();
        topicData.widgetItem->setText(0, path[path_i]);
        topicData.widgetItem->setText(2, path.mid(0, path_i + 1).join("/"));
        setTopicData(topicData.widgetItem, topicData);

        // Add new element to tree, expand it and continue path construction from this element
        current->addChild(topicData.widgetItem);
        current->setExpanded(true);
        current = topicData.widgetItem;
    }

    // Target topic item is now stored in current
    auto topicData = getTopicData(current);

    if(topicData.isSubscribed)
        return 2;

    topicData.isSubscribed = true;
    setTopicData(current, topicData);

    // Subscribe to this topic
    client->subscribe(current->text(2).toStdString(), 1, mqtt::subscribe_options(true))->wait();

    // Make topic name blue so we can easily see which topics are subscribed
    current->setForeground(0, QBrush(Qt::blue));
    return 0;
}

void Explorer::publishData(QString topic, QString data)
{
    // Publish text to server as string
    client->publish(topic.toStdString(), data.toStdString());
    // This message won't be received by our client, process it manually
    processTopicMessage(topic, data, true);
}

void Explorer::setTopicData(QTreeWidgetItem* item, TopicData data)
{
    item->setData(0, Qt::UserRole, qVariantFromValue<TopicData>(data));
}

TopicData Explorer::getTopicData(QTreeWidgetItem* item)
{
    return qvariant_cast<TopicData>(item->data(0, Qt::UserRole));
}

QTreeWidgetItem* Explorer::getSelectedTopic()
{
    auto selected = topicTree->selectedItems();

    if(selected.count() == 0)
        return NULL;

    return selected[0];
}

void Explorer::onTopicSelected()
{
    reloadToggleButton();
    reloadMessageList();
}

void Explorer::reloadToggleButton()
{
    auto selected = getSelectedTopic();
    
    if(selected == NULL)
        return;

    auto topicData = getTopicData(selected);
    
    // Change toggle button text based on topic subscription status
    if(topicData.isSubscribed)
        buttonToggleSubscribe->setText("Unsubscribe selected topic");
    else
        buttonToggleSubscribe->setText("Subscribe selected topic");

    inputPublishMessage->setEnabled(true);
    buttonToggleSubscribe->setEnabled(true);
    buttonPublishImage->setEnabled(true);
    buttonPublish->setEnabled(true);
}

void Explorer::reloadMessageList()
{
    // Clear all items
    topicMessageList->clear();

    auto selected = getSelectedTopic();

    if(selected == NULL)
        return;

    auto topicData = getTopicData(selected);

    // Iterate whole message history and create new items
    for(auto message : topicData.messages)
    {
        auto data = std::get<0>(message);
        auto time = std::get<2>(message);

        auto message_item = new QListWidgetItem();
        message_item->setData(Qt::UserRole, data);

        auto text = time.toString("HH:mm:ss") + ": ";

        // Only string and image type is supported
        if(data.userType() == QMetaType::QString)
            text += qvariant_cast<QString>(data);
        else if(data.userType() == QMetaType::QPixmap)
            text += "[Image]";

        message_item->setText(text);

        // Set color based on the sender of message (1) us (2) anyone else
        auto color = std::get<1>(message) ? QColor(255, 255, 0, 127) : QColor(0, 0, 0, 0);
        message_item->setBackgroundColor(color);

        topicMessageList->addItem(message_item);
    }
}

void Explorer::onToggleButtonClicked()
{
    auto selected = getSelectedTopic();

    if(selected == NULL)
        return;

    auto topicData = getTopicData(selected);

    // If selected topic was subscribed, unsubscribe it
    if(topicData.isSubscribed)
    {
        topicData.isSubscribed = false;
        client->unsubscribe(selected->text(2).toStdString());
        setStatus("Topic unsubscribed", 3);
        // Make topic name black so we can easily see which topics are unsubscribed
        selected->setForeground(0, QBrush(Qt::black));
    }
    //If selected topic was unsubscribed, subscribe it
    else 
    {
        topicData.isSubscribed = true;
        client->subscribe(selected->text(2).toStdString(), 1, mqtt::subscribe_options(true));
        setStatus("Topic subscribed", 3);
        // Make topic name blue so we can easily see which topics are subscribed
        selected->setForeground(0, QBrush(Qt::blue));
    }

    setTopicData(selected, topicData);
    reloadToggleButton();
}

void Explorer::processTopicMessage(QString topic, QVariant data, bool local)
{
    // Try to find topic in topic tree
    auto searchResult = topicTree->findItems(topic, Qt::MatchExactly|Qt::MatchCaseSensitive|Qt::MatchRecursive, 2);
    
    if(searchResult.count() == 0)
        return;

    auto treeTopicItem = searchResult[0];

    auto topicData = getTopicData(treeTopicItem);

    // Only string and image type is supported
    if(data.userType() == QMetaType::QString)
        treeTopicItem->setText(1, qvariant_cast<QString>(data));
    else if(data.userType() == QMetaType::QPixmap)
        treeTopicItem->setText(1, "(Image)");
    else 
        return;

    // Set color based on the sender of message (1) us (2) anyone else
    auto color = local ? QColor(255, 255, 0, 127) : QColor(0, 0, 0, 0);
    treeTopicItem->setBackgroundColor(1, color);

    if(topicData.messages.length() >= MAX_MESSAGES)
        topicData.messages.pop_back();

    topicData.messages.prepend(std::make_tuple(data, local, QDateTime::currentDateTime()));
    setTopicData(treeTopicItem, topicData);

    emit messageReceived(topic, data, local);

    // Reload message list if this topic is currently selected
    auto selected = getSelectedTopic();
    if(selected != NULL && selected == treeTopicItem)
        reloadMessageList();
}

void Explorer::onMessageDoubleClicked(QListWidgetItem* item)
{
    // Get object from item, it will be either String or
    auto data = item->data(Qt::UserRole);

    auto dialog = new QDialog();
    // Create text preview from string
    if(data.userType() == QMetaType::QString)
    {
        Ui_PreviewString preview;
        preview.setupUi(dialog);
        preview.textBrowser->setText(qvariant_cast<QString>(data));
    }
    // Create image preview for image
    else if(data.userType() == QMetaType::QPixmap)
    {
        Ui_PreviewImage preview;
        preview.setupUi(dialog);
        preview.label->setPixmap(qvariant_cast<QPixmap>(data));
        preview.label->setScaledContents(true);
    } else
        return;
    
    // Open dialog and wait until it is closed
    dialog->exec();
}

void Explorer::onPublishButtonClicked()
{
    auto selected = getSelectedTopic();

    if(selected == NULL)
    {
        setStatus("Select topic before publishing!", 3);
        return;
    }

    auto path = selected->text(2);

    // Publish text to server as string
    client->publish(path.toStdString(), inputPublishMessage->text().toStdString());

    // This message won't be received by our client, process it manually
    processTopicMessage(path, inputPublishMessage->text(), true);

    // Clear input
    inputPublishMessage->setText("");
}

void Explorer::onPublishImageButtonClicked()
{
    auto selected = getSelectedTopic();

    if(selected == NULL)
    {
        setStatus("Select topic before publishing!", 3);
        return;
    }

    auto path = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    auto file = QFile(path);
    
    if(!file.open(QIODevice::ReadOnly))
    {
        setStatus("Failed to read file of provided image");
        return;
    }

    // Read whole file into ByteArray and close handler
    auto data = file.readAll();
    file.close();

    QPixmap image;
    // Try to parse this file into pixel map 
    if(!image.loadFromData(data))
    {
        setStatus("Failed to parse provided image");
        return;
    }

    // Publish image to server as raw bytes
    client->publish(selected->text(2).toStdString(), data.data(), data.size());

    // This message won't be received by our client, process it manually
    processTopicMessage(selected->text(2), image, true);
}

void Explorer::onSaveStateButtonClicked()
{
    // Open directory where will be directory structure generated
    auto path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(path.isEmpty())
        return;

    auto directory = QDir(path);
    auto iterator = QTreeWidgetItemIterator(topicTree);

    // Iterate all topics one by one
    while(*iterator) 
    {
        auto topicPath = (*iterator)->text(2);
        auto topicData = getTopicData(*iterator);

        if(!directory.mkpath(topicPath))
        {
            setStatus("Could not create directory sructure");
            return;
        }

        // If the topic did not receive any message, we can skip it
        if(topicData.messages.count() == 0)
        {
            ++iterator;
            continue;
        }

        auto message = topicData.messages.first();

        auto data = std::get<0>(message);

        // If the message is string, create text stream to txt file
        if(data.userType() == QMetaType::QString)
        {
            auto file = QFile(path + "/" + topicPath + "/payload.txt");

            if(!file.open(QIODevice::ReadWrite))
            {
                setStatus("Could not create payload file");
                return;
            }

            auto stream = QTextStream(&file);
            stream << qvariant_cast<QString>(data);
            file.close();
        }
        // If the message is image, save it in JPG format
        else if(data.userType() == QMetaType::QPixmap)
        {
            auto file = QFile(path + "/" + topicPath + "/payload.jpg");

            if(!file.open(QIODevice::ReadWrite))
            {
                setStatus("Could not create payload file");
                return;
            }

            qvariant_cast<QPixmap>(data).save(&file, "JPG");
            file.close();
        }

        ++iterator;
    }
}

void Explorer::onToggleSimulatorButtonClicked()
{
    if(!simulator->isRunning())
    {
        simulator->start(client->get_server_uri());
        buttonToggleSimulator->setText("Stop simulator");
    }
    else
    {
        simulator->stop();
        buttonToggleSimulator->setText("Start simulator on this server");
    }
}

// DASHBOARD

void Explorer::onAddWidgetButtonClicked()
{
    auto widget = new WidgetLightSwitch(this);

    // Initial setup provided by widget failed
    if(!widget->Setup() || !widget->Render()) 
    {
        delete widget;
        return;
    }

    widgets.append(widget);
    flowLayout->addWidget(widget);
}

void Explorer::onLoadDashboardButtonClicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, 
        tr("Load dashboard configuration"), "", 
        tr("All files (*)"));

    QFile file(fileName);
    
    if(!file.open(QIODevice::ReadOnly))
    {
        setStatus("Failed to read file of provided image");
        return;
    }

    // Read whole file into ByteArray and close handler
    auto data = file.readAll();
    file.close();

    auto document = QJsonDocument::fromJson(data);
    if(document.isNull())
    {
        setStatus("Could not parse JSON file");
        return;
    }

    for(auto widget : widgets) 
    {
        flowLayout->removeWidget(widget);
        delete widget;
    }

    widgets.clear();

    auto array = document.object().value("widgets").toArray();
    for(auto widget : array)
    {
        auto widgetName = widget.toObject().value("widget").toString();

        if(!registeredWidgets.contains(widgetName))
            continue;

        auto newWidget = registeredWidgets[widgetName]->create(this);

        // Initial setup provided by widget failed
        if(!newWidget->Setup(widget.toObject()) || !newWidget->Render()) 
        {
            delete newWidget;
            return;
        }

        widgets.append(newWidget);
        flowLayout->addWidget(newWidget);
    }
}

void Explorer::onSaveDashboardButtonClicked()
{
    auto fileName = QFileDialog::getSaveFileName(this,
        tr("Save dashboard configuration"), "",
        tr("All Files (*)"));

    QJsonArray array;
    for(auto widget : widgets)
        array.append(widget->ExtractConfig());

    QJsonObject result;
    result["widgets"] = array;
    QJsonDocument document(result);

    QSaveFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        setStatus("Failed to save configuration file");
        return;
    }

    file.write(document.toJson());
    file.commit();
}