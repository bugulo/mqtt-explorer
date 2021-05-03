/*!
 * @file explorer.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Implementation of Explorer
 */

#include "explorer.h"

#include <algorithm>

#include <QDir>
#include <QFile>
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QVariant>
#include <QTextEdit>
#include <QIODevice>
#include <QMetaType>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QMainWindow>
#include <QPushButton>
#include <QTreeWidget>
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTreeWidgetItemIterator>

#include "extensions/FlowLayout.h"

#include "widgets/widget.h"
#include "simulator.h"
#include "client.h"
#include "utils.h"

#include "widgets/lcd_display/lcd_display.h"
#include "widgets/light_switch/light_switch.h"
#include "widgets/security_camera/security_camera.h"
#include "widgets/thermostat/thermostat.h"

#include "ui_preview_image.h"
#include "ui_preview_string.h"

Explorer::Explorer(int history, QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    
    this->history = history;

    client = new Client(this);

    connect(client, &Client::receivedMessage, this, &Explorer::onReceivedMessage);

    // Setup button click events
    connect(buttonConnect,          &QPushButton::clicked, this, &Explorer::onConnectButtonClicked);
    connect(buttonPublish,          &QPushButton::clicked, this, &Explorer::onPublishButtonClicked);
    connect(buttonSubscribe,        &QPushButton::clicked, this, &Explorer::onSubscribeButtonClicked);
    connect(buttonSaveState,        &QPushButton::clicked, this, &Explorer::onSaveStateButtonClicked);
    connect(buttonDisconnect,       &QPushButton::clicked, this, &Explorer::onDisconnectButtonClicked);
    connect(buttonPublishFile,      &QPushButton::clicked, this, &Explorer::onPublishFileButtonClicked);
    connect(buttonToggleSimulator,  &QPushButton::clicked, this, &Explorer::onToggleSimulatorButtonClicked);
    connect(buttonToggleSubscribe,  &QPushButton::clicked, this, &Explorer::onToggleSubscribeButtonClicked);

    connect(buttonAddWidget,        &QPushButton::clicked, this, &Explorer::onAddWidgetButtonClicked);
    connect(buttonLoadDashboard,    &QPushButton::clicked, this, &Explorer::onLoadDashboardButtonClicked);
    connect(buttonSaveDashboard,    &QPushButton::clicked, this, &Explorer::onSaveDashboardButtonClicked);

    // Setup tree widget selection event
    connect(topicTree, &QTreeWidget::itemSelectionChanged, this, &Explorer::onTopicSelected);

    // Setup message click event in message history
    connect(topicMessageList, &QListWidget::itemDoubleClicked, this, &Explorer::onMessageDoubleClicked);

    // Hide column that stores topic path
    topicTree->hideColumn(2);

    simulator = new Simulator(this);

    flowLayout = new FlowLayout();
    scrollAreaWidgetContents_2->setLayout(flowLayout);

    // Register all supported widgets
    registeredWidgets["LightSwitch"] = new WidgetFactory<WidgetLightSwitch>;
    registeredWidgets["LcdDisplay"] = new WidgetFactory<WidgetLcdDisplay>;
    registeredWidgets["SecurityCamera"] = new WidgetFactory<WidgetSecurityCamera>;
    registeredWidgets["Thermostat"] = new WidgetFactory<WidgetThermostat>;

    // Add all registered widgets to dashboard combobox
    QMapIterator<QString, IWidgetFactory*> i(registeredWidgets);
    while (i.hasNext()) 
    {
        i.next();
        selectWidgetType->addItem(i.key());
    }
}

void Explorer::setStatus(QString message, int seconds)
{
    statusBar()->showMessage(message, seconds * 1000);
}

void Explorer::onConnectButtonClicked()
{
    topicTree->clear();
    clearDashboard();

    address = this->inputServerAddress->text();

    setStatus("Connecting to server...", 0);
    if(!client->connect(address))
    {
        setStatus("Could not connect to this server");
        return;
    }

    buttonConnect->setEnabled(false);
    buttonDisconnect->setEnabled(true);
    inputServerAddress->setEnabled(false);
    tabWidget->setEnabled(true);
    setStatus("Successfuly connected to server");
}

void Explorer::onDisconnectButtonClicked()
{
    client->disconnect();
    simulator->stop();

    buttonConnect->setEnabled(true);
    buttonDisconnect->setEnabled(false);
    inputServerAddress->setEnabled(true);
    tabWidget->setEnabled(false);

    checkboxRelative->setChecked(false);
    buttonToggleSubscribe->setEnabled(false);
    buttonPublishFile->setEnabled(false);
    buttonPublish->setEnabled(false);
    buttonToggleSimulator->setText("Start simulator on this server");

    setStatus("Successfuly disconnected from server!");
}

void Explorer::onReceivedMessage(QString topic, QVariant data, bool local)
{
    // Try to find topic in topic tree
    auto searchResult = topicTree->findItems(topic, Qt::MatchExactly|Qt::MatchCaseSensitive|Qt::MatchRecursive, 2);
    
    if(searchResult.count() == 0)
        return;

    auto treeTopicItem = searchResult[0];

    auto topicData = getTopicData(treeTopicItem);

    // Only string and image type is supported
    if(data.userType() == QMetaType::QString)
        treeTopicItem->setText(1, qvariant_cast<QString>(data).simplified());
    else if(data.userType() == QMetaType::QPixmap)
        treeTopicItem->setText(1, "(Image)");
    else 
        return;

    // Set color based on the sender of message (1) us (2) anyone else
    auto color = local ? QColor(255, 255, 0, 127) : QColor(0, 0, 0, 0);
    treeTopicItem->setBackgroundColor(1, color);

    // If we reached message history limit, remove the oldest message
    if(topicData.messages.length() >= history)
        topicData.messages.pop_back();

    topicData.messages.prepend(std::make_tuple(data, local, QDateTime::currentDateTime()));
    setTopicData(treeTopicItem, topicData);

    emit messageReceived(topic, data, local);

    // Reload message list if this topic is currently selected
    auto selected = getSelectedTopic();
    if(selected != Q_NULLPTR && selected == treeTopicItem)
        reloadMessageList();
}

void Explorer::onSubscribeButtonClicked()
{
    int result;
    
    // If relative subscription is turned on, get selected topic, otherwise working with root
    if(checkboxRelative->isChecked())
    {
        auto selected = getSelectedTopic();
    
        if(selected == Q_NULLPTR)
        {
            setStatus("No topic is selected");
            return;
        }

        result = subscribeTopic(inputSubscribeTopic->text(), selected);
    } else
        result = subscribeTopic(inputSubscribeTopic->text());

    if(result == 1)
    {
        setStatus("Wildcard in topic is not supported");
    }
    else if(result == 2)
    {
        setStatus("You most provide topic before trying to subscribe!");
        return;
    }
    else if(result == 3)
    {
        setStatus("This topic is already subscribed");
        return;
    }
    else
        inputSubscribeTopic->setText("");
}

int Explorer::subscribeTopic(QString topic, QTreeWidgetItem *root)
{
    // Wildcards are not supported
    if(topic.contains("#"))
        return 1;

    // Split specified topic by subtopics, so we can build tree structure
    auto path = topic.split("/", QString::SkipEmptyParts);

    if(path.count() == 0) 
        return 2;

    auto current = root == Q_NULLPTR ? topicTree->invisibleRootItem() : root;

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

        auto targetPath = root == Q_NULLPTR ? 
            path.mid(0, path_i + 1).join("/") : 
            root->text(2) + "/" + path.mid(0, path_i + 1).join("/");

        TopicData topicData;
        topicData.widgetItem = new QTreeWidgetItem();
        topicData.widgetItem->setText(0, path[path_i].simplified());
        topicData.widgetItem->setText(2, targetPath);
        setTopicData(topicData.widgetItem, topicData);

        // Add new element to tree, expand it and continue path construction from this element
        current->addChild(topicData.widgetItem);
        current->setExpanded(true);
        current = topicData.widgetItem;
    }

    // Target topic item is now stored in current
    auto topicData = getTopicData(current);

    if(topicData.isSubscribed)
        return 3;

    topicData.isSubscribed = true;
    setTopicData(current, topicData);

    client->subscribe(current->text(2));

    // Make topic name blue so we can easily see which topics are subscribed
    current->setForeground(0, QBrush(Qt::blue));
    return 0;
}

void Explorer::publishData(QString topic, QString data)
{
    client->publish(topic, data);
}

void Explorer::publishData(QString topic, QByteArray data)
{
    client->publish(topic, data);
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
        return Q_NULLPTR;

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
    
    if(selected == Q_NULLPTR)
        return;

    auto topicData = getTopicData(selected);
    
    // Change toggle button text based on topic subscription status
    if(topicData.isSubscribed)
        buttonToggleSubscribe->setText("Unsubscribe selected topic");
    else
        buttonToggleSubscribe->setText("Subscribe selected topic");

    buttonToggleSubscribe->setEnabled(true);
    buttonPublishFile->setEnabled(true);
    buttonPublish->setEnabled(true);
}

void Explorer::reloadMessageList()
{
    // Clear all items
    topicMessageList->clear();

    auto selected = getSelectedTopic();

    if(selected == Q_NULLPTR)
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
            text += qvariant_cast<QString>(data).simplified();
        else if(data.userType() == QMetaType::QPixmap)
            text += "[Image]";

        message_item->setText(text);

        // Set color based on the sender of message (1) us (2) anyone else
        auto color = std::get<1>(message) ? QColor(255, 255, 0, 127) : QColor(0, 0, 0, 0);
        message_item->setBackgroundColor(color);

        topicMessageList->addItem(message_item);
    }
}

void Explorer::onToggleSubscribeButtonClicked()
{
    auto selected = getSelectedTopic();

    if(selected == Q_NULLPTR)
        return;

    auto topicData = getTopicData(selected);

    // If selected topic was subscribed, unsubscribe it
    if(topicData.isSubscribed)
    {
        topicData.isSubscribed = false;
        //client->unsubscribe(selected->text(2).toStdString());
        client->unsubscribe(selected->text(2));
        setStatus("Topic unsubscribed", 3);
        // Make topic name black so we can easily see which topics are unsubscribed
        selected->setForeground(0, QBrush(Qt::black));
    }
    //If selected topic was unsubscribed, subscribe it
    else 
    {
        topicData.isSubscribed = true;
        //client->subscribe(selected->text(2).toStdString(), 1, mqtt::subscribe_options(true));
        client->subscribe(selected->text(2));
        setStatus("Topic subscribed", 3);
        // Make topic name blue so we can easily see which topics are subscribed
        selected->setForeground(0, QBrush(Qt::blue));
    }

    setTopicData(selected, topicData);
    reloadToggleButton();
}

void Explorer::onMessageDoubleClicked(QListWidgetItem* item)
{
    auto data = item->data(Qt::UserRole);

    if(data.userType() == QMetaType::QString)
        Utils::openText(qvariant_cast<QString>(data), this);
    else if(data.userType() == QMetaType::QPixmap)
        Utils::openImage(qvariant_cast<QPixmap>(data), this);
    else
        return;
}

void Explorer::onPublishButtonClicked()
{
    auto selected = getSelectedTopic();

    if(selected == Q_NULLPTR)
        return;

    auto path = selected->text(2);

    client->publish(path, inputPublishMessage->toPlainText());
    inputPublishMessage->setPlainText("");
}

void Explorer::onPublishFileButtonClicked()
{
    auto selected = getSelectedTopic();

    if(selected == Q_NULLPTR)
        return;

    auto data = Utils::loadFile(this, "Select file", "All files (*)");

    if(data.isEmpty() || data.isNull())
    {
        setStatus("Failed to read file or file is empty");
        return;
    }

    client->publish(selected->text(2), data);
}

void Explorer::onSaveStateButtonClicked()
{
    // Open directory where will be directory structure generated
    auto path = QFileDialog::getExistingDirectory(this, "Select Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

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
            if(!Utils::writeFile(path + "/" + topicPath + "/payload.txt", qvariant_cast<QString>(data).toUtf8()))
            {
                setStatus("Failed to write to file");
                return;
            }
        }
        // If the message is image, save it in JPG format
        else if(data.userType() == QMetaType::QPixmap)
        {
            QFile file(path + "/" + topicPath + "/payload.jpg");

            if(!file.open(QIODevice::ReadWrite))
            {
                setStatus("Failed to write to file");
                return;
            }

            qvariant_cast<QPixmap>(data).save(&file, "JPG");
            file.close();
        }

        ++iterator;
    }
}

// SIMULATOR

void Explorer::onToggleSimulatorButtonClicked()
{
    if(!simulator->isRunning())
    {
        if(simulator->start(address))
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
    auto widget = registeredWidgets[selectWidgetType->currentText()]->create(this);

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
    auto data = Utils::loadFile(this, "Select dashboard configuration", "All files (*)");

    auto document = QJsonDocument::fromJson(data);
    if(document.isNull())
    {
        setStatus("Could not parse JSON file");
        return;
    }

    clearDashboard();

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
    QJsonArray array;
    for(auto widget : widgets)
        array.append(widget->ExtractConfig());

    QJsonObject result;
    result["widgets"] = array;
    QJsonDocument document(result);
    
    if(!Utils::saveFile(this, document.toJson(), "Save dashboard configuration", "All Files (*)"))
        setStatus("Failed to save configuration file");
}

void Explorer::clearDashboard()
{
    for(auto widget : widgets) 
    {
        flowLayout->removeWidget(widget);
        delete widget;
    }

    widgets.clear();
}

void Explorer::removeWidget(Widget *widget)
{
    widgets.removeOne(widget);
    flowLayout->removeWidget(widget);
    delete widget;
}

Explorer::~Explorer()
{
    delete client;
    delete simulator;
    delete flowLayout;

    for(auto widget : widgets)
        delete widget;
}